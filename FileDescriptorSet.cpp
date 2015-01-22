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

#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>

#include "IFileDescriptor.h"

#include "FileDescriptorSet.h"

namespace MarathonKit {

void FileDescriptorSet::add(const IFileDescriptor& fd) {
  fd.addToSet(*this);
}

void FileDescriptorSet::remove(const IFileDescriptor& fd) {
  fd.removeFromSet(*this);
}

bool FileDescriptorSet::contains(const IFileDescriptor& fd) const {
  return fd.isInSet(*this);
}

void FileDescriptorSet::add(int fd) {
  if (fd < 0) {
    throw std::runtime_error(
        "Invalid file descriptor cannot be added to a set");
  }
  fds.insert(fd);
}

void FileDescriptorSet::remove(int fd) {
  if (fd < 0) {
    throw std::runtime_error(
        "Invalid file descriptor cannot be removed from a set");
  }
  fds.erase(fd);
}

bool FileDescriptorSet::contains(int fd) const {
  return fds.count(fd) > 0;
}

void FileDescriptorSet::selectReadable() {
  int nfds = 0;
  fd_set readFds;
  FD_ZERO(&readFds);

  for (int fd : fds) {
    nfds = std::max(nfds, fd + 1);
    FD_SET(fd, &readFds);
  }

  int rc = select(nfds, &readFds, nullptr, nullptr, nullptr);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }

  std::set<int> newFds;
  for (int fd : fds) {
    if (FD_ISSET(fd, &readFds)) {
      newFds.insert(fd);
    }
  }
  swap(fds, newFds);
}

}
