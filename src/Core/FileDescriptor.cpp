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

#include "Core/FileDescriptor.h"

namespace MarathonKit {
namespace Core {

using std::string;
using std::swap;

FileDescriptor::FileDescriptor():
  mFd(-1),
  mMode(Mode::STREAM) {}

FileDescriptor::FileDescriptor(int fd, Mode mode):
  mFd(fd),
  mMode(mode) {}

FileDescriptor::FileDescriptor(const FileDescriptor& other):
  mFd(-1),
  mMode(other.mMode) {
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
  mFd(-1),
  mMode(Mode::STREAM) {
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
  swap(mMode, other.mMode);
}

bool FileDescriptor::isValid() const {
  return mFd >= 0;
}

bool FileDescriptor::isReadyForReading() const {
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  fd_set readFds;
  FD_ZERO(&readFds);
  FD_SET(mFd, &readFds);

  int nfds = mFd + 1;

  int rc = select(nfds, &readFds, nullptr, nullptr, &timeout);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }

  return FD_ISSET(mFd, &readFds);
}

string FileDescriptor::read() const {
  if (mFd < 0) {
    throw std::runtime_error("read called on an invalid file descriptor");
  }
  switch (mMode) {
    case Mode::STREAM:
      return readStream();
    case Mode::MESSAGE:
      return readMessage();
    default:
      throw std::runtime_error("FileDescriptor has an invalid mode set");
  }
}

string FileDescriptor::readStream() const {
  const int BUFF_SIZE = 4096;
  char buff[BUFF_SIZE];
  ssize_t rc = ::read(mFd, buff, BUFF_SIZE);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return string(buff, static_cast<size_t>(rc));
}

string FileDescriptor::readMessage() const {
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

void FileDescriptor::write(const string& data) const {
  if (mFd < 0) {
    throw std::runtime_error("write called on an invalid file descriptor");
  }
  switch (mMode) {
    case Mode::STREAM:
      writeStream(data);
      break;
    case Mode::MESSAGE:
      writeMessage(data);
      break;
    default:
      throw std::runtime_error("FileDescriptor has an invalid mode set");
  }
}

void FileDescriptor::writeStream(const string& data) const {
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

void FileDescriptor::writeMessage(const string& data) const {
  ssize_t rc = ::send(mFd, data.c_str(), data.size(), 0);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
}

FileDescriptor FileDescriptor::createOwnerOf(int fd, Mode mode) {
  return FileDescriptor(fd, mode);
}

FileDescriptor FileDescriptor::createCopyOf(int fd, Mode mode) {
  int fdCopy = dup(fd);
  if (fdCopy < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return FileDescriptor(fdCopy, mode);
}

void swap(FileDescriptor& fd1, FileDescriptor& fd2) {
  fd1.swapWith(fd2);
}

}}
