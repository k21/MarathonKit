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

  bool hasFreeCapacity() const;

  FileDescriptor mFd;
  Pipe mExitPipe;

  LineBuffer mBuffer;
  std::mutex mBufferMutex;
  std::condition_variable mDataReadyCondition;
  std::condition_variable mFreeCapacityCondition;
  bool mStopRequested;

  size_t mCharCapacity;
  size_t mLineCapacity;

  bool mException;
  std::string mExceptionMessage;

  std::thread mThread;

};

}

#endif
