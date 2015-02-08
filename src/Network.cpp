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

#include "LogMacro.h"

#include "Network.h"

namespace MarathonKit {

FileDescriptor Network::createTcpConnection(
  const std::string& host,
  const std::string& service) {
  LOGI("Trying to connect to ", host, ":", service, " using TCP...");
  addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  addrinfo* infos;
  int rc = getaddrinfo(host.c_str(), service.c_str(), &hints, &infos);
  if (rc != 0) {
    throw std::runtime_error(gai_strerror(rc));
  }
  if (infos == nullptr) {
    LOGE("Unknown host ", host, ":", service);
  }
  addrinfo* info = infos;
  FileDescriptor fd;
  while (info != nullptr) {
    int socketFd = socket(
        info->ai_family,
        info->ai_socktype,
        info->ai_protocol);
    if (socketFd < 0) {
      // TODO: use inet_ntop and print the actual address we tried to connect to
      LOGW(
          "Connection attempt to ", host, ":", service, " failed: ",
          std::strerror(errno));
      info = info->ai_next;
      continue;
    }
    rc = ::connect(socketFd, info->ai_addr, info->ai_addrlen);
    if (rc != 0) {
      // TODO: use inet_ntop and print the actual address we tried to connect to
      LOGW(
          "Connection attempt to ", host, ":", service, " failed: ",
          std::strerror(errno));
      info = info->ai_next;
      continue;
    }
    fd = FileDescriptor::createOwnerOf(socketFd, FileDescriptor::Mode::STREAM);
    break;
  }
  freeaddrinfo(infos);
  if (!fd.isValid()) {
    throw std::runtime_error("Could not connect to " + host + ":" + service);
  }
  LOGI("Connection attempt to ", host, ":", service, " was successful");
  return std::move(fd);
}

FileDescriptor Network::createUdpListener(const std::string& service) {
  LOGI("Trying to listen for UDP datagrams on service port ", service, "...");
  addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_flags = AI_PASSIVE;
  addrinfo* infos;
  int rc = getaddrinfo(nullptr, service.c_str(), &hints, &infos);
  if (rc != 0) {
    throw std::runtime_error(gai_strerror(rc));
  }
  if (infos == nullptr) {
    LOGE("Unknown service port ", service);
  }
  addrinfo* info = infos;
  FileDescriptor fd;
  while (info != nullptr) {
    int socketFd = socket(
        info->ai_family,
        info->ai_socktype,
        info->ai_protocol);
    if (socketFd < 0) {
      // TODO: use inet_ntop and print the actual address we tried to connect to
      LOGW(
          "Listening on service port ", service, " failed: ",
          std::strerror(errno));
      info = info->ai_next;
      continue;
    }
    rc = ::bind(socketFd, info->ai_addr, info->ai_addrlen);
    if (rc != 0) {
      // TODO: use inet_ntop and print the actual address we tried to connect to
      LOGW(
          "Listening on service port ", service, " failed: ",
          std::strerror(errno));
      info = info->ai_next;
      continue;
    }
    fd = FileDescriptor::createOwnerOf(socketFd, FileDescriptor::Mode::MESSAGE);
    break;
  }
  freeaddrinfo(infos);
  if (!fd.isValid()) {
    throw std::runtime_error("Could not listen on service port " + service);
  }
  LOGI("Successfully listening on service port ", service);
  return std::move(fd);
}

}
