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

  mExitPipe.getWriteFd().write("X");
  mThread.join();

  mFd = FileDescriptor();
  mExitPipe = Pipe();
  mBuffer.clear();
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
  return mBuffer.getChar();
}

string AsyncReader::getLine() {
  std::unique_lock<std::mutex> lock(mBufferMutex);
  while (!mException && mBuffer.linesReady() == 0) {
    mDataReadyCondition.wait(lock);
  }
  if (mBuffer.linesReady() == 0) {
    throw std::runtime_error(mExceptionMessage);
  }
  return mBuffer.getLine();
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

}
