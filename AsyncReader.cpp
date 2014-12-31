#include "AsyncReader.h"

#include <set>

namespace MarathonKit {

using std::max;
using std::string;

AsyncReader::AsyncReader():
  mFd(),
  mExitPipe(),
  mLinesReady(0),
  mBuffer(),
  mBufferMutex(),
  mDataReadyCondition(),
  mException(false),
  mExceptionMessage(),
  mThread() {}

AsyncReader::AsyncReader(const FileDescriptor& fd):
  mFd(fd),
  mExitPipe(),
  mLinesReady(0),
  mBuffer(),
  mBufferMutex(),
  mDataReadyCondition(),
  mException(false),
  mExceptionMessage(),
  mThread(std::bind(&AsyncReader::backgroundThread, this)) {}

AsyncReader::~AsyncReader() {
  if (isRunning()) {
    mExitPipe.getWriteFd().write("X");
    mThread.join();
  }
}

void AsyncReader::start(const FileDescriptor& fd) {
  if (isRunning()) {
    throw std::runtime_error("start called but AsyncReader is already running");
  }

  mFd = fd;
  mThread = std::thread(std::bind(&AsyncReader::backgroundThread, this));
}

bool AsyncReader::isRunning() const {
  return mThread.joinable();
}

size_t AsyncReader::charsReady() {
  std::lock_guard<std::mutex> lock(mBufferMutex);
  return mBuffer.size();
}

size_t AsyncReader::linesReady() {
  std::lock_guard<std::mutex> lock(mBufferMutex);
  return mLinesReady;
}

char AsyncReader::getChar() {
  std::unique_lock<std::mutex> lock(mBufferMutex);
  while (!mException && mBuffer.size() < 1) {
    mDataReadyCondition.wait(lock);
  }
  if (mBuffer.size() < 1) {
    throw std::runtime_error(mExceptionMessage);
  }
  char ch = mBuffer.front();
  mBuffer.pop_front();
  if (ch == '\n') {
    --mLinesReady;
  }
  return ch;
}

string AsyncReader::getLine() {
  std::unique_lock<std::mutex> lock(mBufferMutex);
  while (!mException && mLinesReady < 1) {
    mDataReadyCondition.wait(lock);
  }
  if (mLinesReady < 1) {
    throw std::runtime_error(mExceptionMessage);
  }
  string line;
  while (mBuffer.front() != '\n') {
    line += mBuffer.front();
    mBuffer.pop_front();
  }
  mBuffer.pop_front();
  --mLinesReady;
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
        break;
      }
      if (!fdSet.contains(mFd)) {
        continue;
      }

      string buff = mFd.read();
      if (buff.empty()) {
        throw std::runtime_error("There is no more data to read");
      }

      std::lock_guard<std::mutex> lock(mBufferMutex);
      for (char ch : buff) {
        mBuffer.push_back(ch);
        if (ch == '\n') {
          ++mLinesReady;
        }
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

}
