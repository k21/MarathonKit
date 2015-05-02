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

#include "Core/StreamFileDescriptor.h"

namespace MarathonKit {
namespace Core {

using std::string;
using std::unique_ptr;

StreamFileDescriptor::StreamFileDescriptor(int fd):
  mFd(fd) {
  if (fd < 0) {
    throw std::runtime_error(
        "Invalid descriptor in StreamFileDescriptor constructor");
  }
}

StreamFileDescriptor::~StreamFileDescriptor() {
  close(mFd);
}

bool StreamFileDescriptor::isReadyForReading() const {
  return FileDescriptor::isReadyForReading(mFd);
}

string StreamFileDescriptor::read() const {
  const int BUFF_SIZE = 4096;
  char buff[BUFF_SIZE];
  ssize_t rc = ::read(mFd, buff, BUFF_SIZE);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return string(buff, static_cast<size_t>(rc));
}

void StreamFileDescriptor::write(const string& data) const {
  const char* buff = data.c_str();
  size_t offset = 0;
  while (offset < data.size()) {
    ssize_t rc = ::write(mFd, buff + offset, data.size() - offset);
    if (rc < 0) {
      throw std::runtime_error(std::strerror(errno));
    }
    offset += static_cast<size_t>(rc);
  }
}

unique_ptr<StreamFileDescriptor> StreamFileDescriptor::createOwnerOf(int fd) {
  return unique_ptr<StreamFileDescriptor>(new StreamFileDescriptor(fd));
}

unique_ptr<StreamFileDescriptor> StreamFileDescriptor::createCopyOf(int fd) {
  int fdCopy = dup(fd);
  if (fdCopy < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return unique_ptr<StreamFileDescriptor>(new StreamFileDescriptor(fdCopy));
}

}}
