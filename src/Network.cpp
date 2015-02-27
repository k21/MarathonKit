/*
 * This file is part of MarathonKit.
 * Copyright (C) 2015 Jakub Zika
 *
 * MarathonKit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * I am providing code in this repository to you under an open source license.
 * Because this is my personal repository, the license you receive to my code is
 * from me and not from my employer (Facebook).
 */

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <functional>
#include <stdexcept>

#include "LogMacro.h"

#include "Network.h"

namespace MarathonKit {

static int getAiFamily(Network::Family family);
static int getAiSocketType(Network::Protocol protocol);
static int getAiProtocol(Network::Protocol protocol);
static int getAiFlags(Network::Mode mode);

enum class LoopControl {
  CONTINUE,
  BREAK,
};

static void forEachAddressInfo(
    const std::string& host,
    const std::string& service,
    Network::Family family,
    Network::Protocol protocol,
    Network::Mode mode,
    std::function<LoopControl(const addrinfo*)> callback);

FileDescriptor Network::createTcpConnection(
  const std::string& host,
  const std::string& service) {
  LOGI("Trying to connect to ", host, ":", service, " using TCP...");
  FileDescriptor fd;
  bool anyTried = false;
  forEachAddressInfo(
      host,
      service,
      Network::Family::ANY,
      Network::Protocol::TCP,
      Network::Mode::ACTIVE,
      [host, service, &fd, &anyTried](const addrinfo* info) -> LoopControl {
        anyTried = true;
        int socketFd = socket(
            info->ai_family,
            info->ai_socktype,
            info->ai_protocol);
        if (socketFd < 0) {
          // TODO: use inet_ntop and print the actual address
          LOGW(
              "Connection attempt to ", host, ":", service, " failed: ",
              std::strerror(errno));
          return LoopControl::CONTINUE;
        }
        int rc = ::connect(socketFd, info->ai_addr, info->ai_addrlen);
        if (rc != 0) {
          // TODO: use inet_ntop and print the actual address
          LOGW(
              "Connection attempt to ", host, ":", service, " failed: ",
              std::strerror(errno));
          close(socketFd);
          return LoopControl::CONTINUE;
        }
        fd = FileDescriptor::createOwnerOf(
            socketFd,
            FileDescriptor::Mode::STREAM);
        return LoopControl::BREAK;
      });
  if (!anyTried) {
    LOGE("Unknown host ", host, ":", service);
  }
  if (!fd.isValid()) {
    throw std::runtime_error("Could not connect to " + host + ":" + service);
  }
  LOGI("Connection attempt to ", host, ":", service, " was successful");
  return std::move(fd);
}

FileDescriptor Network::createUdpListener(const std::string& service) {
  LOGI("Trying to listen for UDP datagrams on service port ", service, "...");
  FileDescriptor fd;
  bool anyTried = false;
  forEachAddressInfo(
      /* host = */ "",
      service,
      Network::Family::ANY,
      Network::Protocol::UDP,
      Network::Mode::PASSIVE,
      [service, &fd, &anyTried](const addrinfo* info) -> LoopControl {
        int socketFd = socket(
            info->ai_family,
            info->ai_socktype,
            info->ai_protocol);
        if (socketFd < 0) {
          // TODO: use inet_ntop and print the actual address
          LOGW(
              "Listening on service port ", service, " failed: ",
              std::strerror(errno));
          return LoopControl::CONTINUE;
        }
        int rc = ::bind(socketFd, info->ai_addr, info->ai_addrlen);
        if (rc != 0) {
          // TODO: use inet_ntop and print the actual address
          LOGW(
              "Listening on service port ", service, " failed: ",
              std::strerror(errno));
          close(socketFd);
          return LoopControl::CONTINUE;
        }
        fd = FileDescriptor::createOwnerOf(
            socketFd,
            FileDescriptor::Mode::MESSAGE);
        return LoopControl::BREAK;
      });
  if (!anyTried) {
    LOGE("Unknown service port ", service);
  }
  if (!fd.isValid()) {
    throw std::runtime_error("Could not listen on service port " + service);
  }
  LOGI("Successfully listening on service port ", service);
  return std::move(fd);
}

static void forEachAddressInfo(
    const std::string& host,
    const std::string& service,
    Network::Family family,
    Network::Protocol protocol,
    Network::Mode mode,
    std::function<LoopControl(const addrinfo*)> callback) {
  addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = getAiFamily(family);
  hints.ai_socktype = getAiSocketType(protocol);
  hints.ai_protocol = getAiProtocol(protocol);
  hints.ai_flags = getAiFlags(mode);
  addrinfo* infos;
  int rc = getaddrinfo(host.c_str(), service.c_str(), &hints, &infos);
  if (rc != 0) {
    throw std::runtime_error(gai_strerror(rc));
  }
  try {
    for (addrinfo* info = infos; info != nullptr; info = info->ai_next) {
      LoopControl loopControl = callback(info);
      if (loopControl == LoopControl::BREAK) {
        break;
      }
    }
  } catch (...) {
    freeaddrinfo(infos);
    throw;
  }
  freeaddrinfo(infos);
}

static int getAiFamily(Network::Family family) {
  switch (family) {
    case Network::Family::ANY:
      return PF_UNSPEC;
    case Network::Family::IP_V4:
      return AF_INET;
    case Network::Family::IP_V6:
      return AF_INET6;
  }
  throw std::runtime_error("Invalid family");
}

static int getAiSocketType(Network::Protocol protocol) {
  switch (protocol) {
    case Network::Protocol::TCP:
      return SOCK_STREAM;
    case Network::Protocol::UDP:
      return SOCK_DGRAM;
  }
  throw std::runtime_error("Invalid protocol");
}

static int getAiProtocol(Network::Protocol protocol) {
  switch (protocol) {
    case Network::Protocol::TCP:
      return IPPROTO_TCP;
    case Network::Protocol::UDP:
      return IPPROTO_UDP;
  }
  throw std::runtime_error("Invalid protocol");
}

static int getAiFlags(Network::Mode mode) {
  switch (mode) {
    case Network::Mode::ACTIVE:
      return 0;
    case Network::Mode::PASSIVE:
      return AI_PASSIVE;
  }
  throw std::runtime_error("Invalid mode");
}

}
