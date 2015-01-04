#ifndef MARATHON_KIT_ASYNC_READER_H_
#define MARATHON_KIT_ASYNC_READER_H_

#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include "FileDescriptor.h"
#include "Pipe.h"

namespace MarathonKit {

class AsyncReader {
public:

  AsyncReader();
  ~AsyncReader();

  void start(const FileDescriptor& fd);
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
  const Pipe mExitPipe;

  size_t mLinesReady;

  std::deque<char> mBuffer;
  std::mutex mBufferMutex;
  std::condition_variable mDataReadyCondition;

  bool mException;
  std::string mExceptionMessage;

  std::thread mThread;

};

}

#endif
