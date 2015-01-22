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

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <cstring>

#include "AsyncReader.h"

#include "TcpClient.h"

namespace MarathonKit {

using std::istringstream;
using std::string;
using std::swap;

TcpClient::TcpClient():
  mFd(),
  mLineBuffer() {}

TcpClient::TcpClient(const std::string& host, const std::string& service):
  mFd(),
  mLineBuffer() {
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
  addrinfo* info = infos;
  while (info != nullptr) {
    int socketFd = socket(
        info->ai_family,
        info->ai_socktype,
        info->ai_protocol);
    if (socketFd < 0) {
      // TODO: log error
      info = info->ai_next;
      continue;
    }
    rc = ::connect(socketFd, info->ai_addr, info->ai_addrlen);
    if (rc != 0) {
      // TODO: log error
      info = info->ai_next;
      continue;
    }
    mFd = FileDescriptor::createOwnerOf(socketFd);
    break;
  }
  freeaddrinfo(infos);
  if (!mFd.isValid()) {
    throw std::runtime_error("Could not connect to " + host + ":" + service);
  }
  std::unique_ptr<AsyncReader> asyncReader(new AsyncReader(mFd));
  mLineBuffer = LineBuffer(std::move(asyncReader));
}

bool TcpClient::isConnected() const {
  return mLineBuffer.isInitialized();
}

void TcpClient::sendLine(const string& line) {
  mFd.write(line + '\n');
}

void TcpClient::sendRaw(const string& data) {
  mFd.write(data);
}

size_t TcpClient::charsReady() {
  return mLineBuffer.charsReady();
}

size_t TcpClient::linesReady() {
  return mLineBuffer.linesReady();
}

char TcpClient::getChar() {
  return mLineBuffer.getChar();
}

string TcpClient::getLine() {
  return mLineBuffer.getLine();
}

}
