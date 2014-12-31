#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>

#include "FileDescriptor.h"

namespace MarathonKit {

using std::string;
using std::swap;

FileDescriptor::FileDescriptor():
  mFd(-1) {}

FileDescriptor::FileDescriptor(int fd):
  mFd(fd) {}

FileDescriptor::FileDescriptor(const FileDescriptor& other):
  mFd(-1) {
  mFd = dup(other.mFd);
  if (mFd < 0) {
    throw std::runtime_error(std::strerror(errno));
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
  const int BUFF_SIZE = 4096;
  char buff[BUFF_SIZE];
  ssize_t rc = ::read(mFd, buff, BUFF_SIZE);
  if (rc < 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  return string(buff, static_cast<size_t>(rc));
}

void FileDescriptor::write(const string& data) const {
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

void FileDescriptor::Set::add(const FileDescriptor& fd) {
  fds.insert(fd.mFd);
}

bool FileDescriptor::Set::contains(const FileDescriptor& fd) const {
  return fds.count(fd.mFd) > 0;
}

void FileDescriptor::Set::selectReadable() {
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

void swap(FileDescriptor& fd1, FileDescriptor& fd2) {
  fd1.swapWith(fd2);
}

}
