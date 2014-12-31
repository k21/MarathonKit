#ifndef MARATHON_KIT_PIPE_H_
#define MARATHON_KIT_PIPE_H_

#include "FileDescriptor.h"

namespace MarathonKit {

class Pipe {
public:

  Pipe();

  const FileDescriptor& getReadFd() const {
    return mReadFd;
  }

  const FileDescriptor& getWriteFd() const {
    return mWriteFd;
  }

private:

  FileDescriptor mReadFd;
  FileDescriptor mWriteFd;

};

}

#endif
