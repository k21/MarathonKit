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

#include "FileDescriptor.h"
#include "FileDescriptorSet.h"

namespace MarathonKit {

using std::string;
using std::swap;

FileDescriptor::FileDescriptor():
  mFd(-1) {}

FileDescriptor::FileDescriptor(int fd):
  mFd(fd) {}

FileDescriptor::FileDescriptor(const FileDescriptor& other):
  mFd(-1) {
  if (other.mFd >= 0) {
    mFd = dup(other.mFd);
    if (mFd < 0) {
      throw std::runtime_error(std::strerror(errno));
    }
  }
}

FileDescriptor& FileDescriptor::operator = (const FileDescriptor& other) {
  FileDescriptor copy(other);
  swap(*this, copy);
  return *this;
}

FileDescriptor::FileDescriptor(FileDescriptor&& other):
  mFd(-1) {
  swap(*this, other);
}

FileDescriptor& FileDescriptor::operator = (FileDescriptor&& other) {
  swap(*this, other);
  return *this;
}

FileDescriptor::~FileDescriptor() {
  if (mFd >= 0) {
    close(mFd);
  }
}

void FileDescriptor::swapWith(FileDescriptor& other) {
  swap(mFd, other.mFd);
}

bool FileDescriptor::isValid() const {
  return mFd >= 0;
}

string FileDescriptor::read() const {
  if (mFd < 0) {
    throw std::runtime_error("read called on an invalid file descriptor");
  }
  const int BUFF_SIZE = 4096;
  char buff[BUFF_SIZE];
  ssize_t rc = ::read(mFd, buff, BUFF_SIZE);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return string(buff, static_cast<size_t>(rc));
}

void FileDescriptor::write(const string& data) const {
  if (mFd < 0) {
    throw std::runtime_error("write called on an invalid file descriptor");
  }
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

void FileDescriptor::addToSet(FileDescriptorSet& set) const {
  set.add(mFd);
}

void FileDescriptor::removeFromSet(FileDescriptorSet& set) const {
  set.remove(mFd);
}

bool FileDescriptor::isInSet(const FileDescriptorSet& set) const {
  return set.contains(mFd);
}

FileDescriptor FileDescriptor::createOwnerOf(int fd) {
  return FileDescriptor(fd);
}

FileDescriptor FileDescriptor::createCopyOf(int fd) {
  int fdCopy = dup(fd);
  if (fdCopy < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return FileDescriptor(fdCopy);
}

void swap(FileDescriptor& fd1, FileDescriptor& fd2) {
  fd1.swapWith(fd2);
}

}
