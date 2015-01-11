#ifndef MARATHON_KIT_ASYNC_READER_H_
#define MARATHON_KIT_ASYNC_READER_H_

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include "FileDescriptor.h"
#include "Pipe.h"
#include "LineBuffer.h"

namespace MarathonKit {

class AsyncReader {
public:

  AsyncReader();
  ~AsyncReader();

  void start(const FileDescriptor& fd);
  void stop();
  bool isRunning() const;

  size_t charsReady();
  size_t linesReady();

  char getChar();
  std::string getLine();

private:

  AsyncReader(const AsyncReader&) = delete;
  AsyncReader& operator = (const AsyncReader&) = delete;

  void backgroundThread();

  FileDescriptor mFd;
  Pipe mExitPipe;

  LineBuffer mBuffer;
  std::mutex mBufferMutex;
  std::condition_variable mDataReadyCondition;

  bool mException;
  std::string mExceptionMessage;

  std::thread mThread;

};

}

#endif
