#include "AsyncReader.h"

#include <set>

namespace MarathonKit {

using std::max;
using std::string;

AsyncReader::AsyncReader():
  mFd(),
  mExitPipe(),
  mBuffer(),
  mBufferMutex(),
  mDataReadyCondition(),
  mFreeCapacityCondition(),
  mStopRequested(false),
  mCharCapacity(4096),
  mLineCapacity(8),
  mException(false),
  mExceptionMessage(),
  mThread() {}

AsyncReader::~AsyncReader() {
  if (isRunning()) {
    stop();
  }
}

void AsyncReader::start(const FileDescriptor& fd) {
  if (isRunning()) {
    throw std::runtime_error("start called but AsyncReader is already running");
  }

  mFd = fd;
  mThread = std::thread(std::bind(&AsyncReader::backgroundThread, this));
}

void AsyncReader::stop() {
  if (!isRunning()) {
    throw std::runtime_error("stop called but AsyncReader is not running");
  }

  {
    std::lock_guard<std::mutex> lock(mBufferMutex);
    mStopRequested = true;
  }
  mExitPipe.getWriteFd().write("X");
  mFreeCapacityCondition.notify_all();
  mThread.join();

  mFd = FileDescriptor();
  mExitPipe = Pipe();
  mBuffer.clear();
  mStopRequested = false;
  mException = false;
  mExceptionMessage = "";
  mThread = std::thread();
}

bool AsyncReader::isRunning() const {
  return mThread.joinable();
}

size_t AsyncReader::charsReady() {
  std::lock_guard<std::mutex> lock(mBufferMutex);
  return mBuffer.charsReady();
}

size_t AsyncReader::linesReady() {
  std::lock_guard<std::mutex> lock(mBufferMutex);
  return mBuffer.linesReady();
}

char AsyncReader::getChar() {
  std::unique_lock<std::mutex> lock(mBufferMutex);
  while (!mException && mBuffer.charsReady() == 0) {
    mDataReadyCondition.wait(lock);
  }
  if (mBuffer.charsReady() == 0) {
    throw std::runtime_error(mExceptionMessage);
  }
  char ch = mBuffer.getChar();
  mFreeCapacityCondition.notify_all();
  return ch;
}

string AsyncReader::getLine() {
  std::unique_lock<std::mutex> lock(mBufferMutex);
  while (!mException && mBuffer.linesReady() == 0) {
    mDataReadyCondition.wait(lock);
  }
  if (mBuffer.linesReady() == 0) {
    throw std::runtime_error(mExceptionMessage);
  }
  string line = mBuffer.getLine();
  mFreeCapacityCondition.notify_all();
  return line;
}

void AsyncReader::backgroundThread() {
  try {
    const FileDescriptor& exitFd = mExitPipe.getReadFd();
    while (true) {
      FileDescriptor::Set fdSet;
      fdSet.add(exitFd);
      fdSet.add(mFd);

      fdSet.selectReadable();

      if (fdSet.contains(exitFd)) {
        return;
      }
      if (!fdSet.contains(mFd)) {
        continue;
      }

      string buff = mFd.read();
      if (buff.empty()) {
        throw std::runtime_error("There is no more data to read");
      }

      std::unique_lock<std::mutex> lock(mBufferMutex);
      for (char ch : buff) {
        while (!hasFreeCapacity() && !mStopRequested) {
          mFreeCapacityCondition.wait(lock);
        }
        if (!hasFreeCapacity()) {
          return;
        }
        mBuffer.putChar(ch);
      }
      mDataReadyCondition.notify_all();
    }
  } catch (std::exception& e) {
    std::lock_guard<std::mutex> lock(mBufferMutex);
    mException = true;
    mExceptionMessage = e.what();
    mDataReadyCondition.notify_all();
  }
}

bool AsyncReader::hasFreeCapacity() const {
  return
    mBuffer.charsReady() < mCharCapacity ||
    mBuffer.linesReady() < mLineCapacity;
}

}
