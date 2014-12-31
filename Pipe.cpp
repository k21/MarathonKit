#include <cerrno>
#include <cstring>
#include <unistd.h>

#include <stdexcept>

#include "Pipe.h"

namespace MarathonKit {

Pipe::Pipe():
  mReadFd(),
  mWriteFd() {
  int fildes[2];
  int rc = pipe(fildes);
  if (rc != 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  mReadFd = FileDescriptor::createOwnerOf(fildes[0]);
  mWriteFd = FileDescriptor::createOwnerOf(fildes[1]);
}

}
