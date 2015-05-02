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

#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

#include "LogMacro.h"

#include "Core/MessageFileDescriptor.h"

namespace MarathonKit {
namespace Core {

using std::string;
using std::unique_ptr;

MessageFileDescriptor::MessageFileDescriptor(int fd):
  mFd(fd) {
  if (fd < 0) {
    throw std::runtime_error(
        "Invalid descriptor in MessageFileDescriptor constructor");
  }
}

MessageFileDescriptor::~MessageFileDescriptor() {
  close(mFd);
}

bool MessageFileDescriptor::isReadyForReading() const {
  return FileDescriptor::isReadyForReading(mFd);
}

string MessageFileDescriptor::read() const {
  std::vector<char> buffer(32, '\0');
  bool enoughSpace = false;
  while (!enoughSpace) {
    ssize_t rc = ::recv(mFd, buffer.data(), buffer.size(), MSG_PEEK);
    if (rc < 0) {
      throw std::runtime_error(std::strerror(errno));
    }
    if (static_cast<size_t>(rc) < buffer.size()) {
      enoughSpace = true;
    } else {
      buffer = std::vector<char>(4 * buffer.size(), '\0');
    }
  }
  ssize_t rc = ::recv(mFd, buffer.data(), buffer.size(), 0);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return string(buffer.data(), static_cast<size_t>(rc));
}

void MessageFileDescriptor::write(const string& data) const {
  ssize_t rc = ::send(mFd, data.c_str(), data.size(), 0);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
}

unique_ptr<MessageFileDescriptor> MessageFileDescriptor::createOwnerOf(int fd) {
  return unique_ptr<MessageFileDescriptor>(new MessageFileDescriptor(fd));
}

unique_ptr<MessageFileDescriptor> MessageFileDescriptor::createCopyOf(int fd) {
  int fdCopy = dup(fd);
  if (fdCopy < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return unique_ptr<MessageFileDescriptor>(new MessageFileDescriptor(fdCopy));
}

}}
