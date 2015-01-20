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
  return mBuffer.size();
}

char AsyncReader::getChar() {
  std::unique_lock<std::mutex> lock(mBufferMutex);
  while (!mException && mBuffer.empty()) {
    mDataReadyCondition.wait(lock);
  }
  if (mBuffer.empty()) {
    throw std::runtime_error(mExceptionMessage);
  }
  char ch = mBuffer.front();
  mBuffer.pop_front();
  mFreeCapacityCondition.notify_all();
  return ch;
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
        mBuffer.push_back(ch);
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
  return mBuffer.size() < mCharCapacity;
}

}
