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

#include <stdexcept>

#include "LogMacro.h"

#include "Core/FileDescriptor.h"

#include "Core/LineBuffer.h"

namespace MarathonKit {
namespace Core {

using std::shared_ptr;
using std::swap;

LineBuffer::LineBuffer():
  mFd(),
  mBuffer(),
  mLinesReady(0) {}

LineBuffer::LineBuffer(const shared_ptr<FileDescriptor>& fd):
  mFd(fd),
  mBuffer(),
  mLinesReady(0) {}

LineBuffer::LineBuffer(LineBuffer&& other):
  mFd(),
  mBuffer(),
  mLinesReady(0) {
  swapWith(other);
}

LineBuffer& LineBuffer::operator = (LineBuffer&& other) {
  swapWith(other);
  return *this;
}

void LineBuffer::swapWith(LineBuffer& other) {
  swap(mFd, other.mFd);
  swap(mBuffer, other.mBuffer);
  swap(mLinesReady, other.mLinesReady);
}

bool LineBuffer::isInitialized() const {
  return mFd != nullptr;
}

size_t LineBuffer::charsReady() {
  if (!isInitialized()) {
    LOGW("charsReady called on unitialized LineBuffer");
    return 0;
  }

  if (mBuffer.empty() && mFd->isReadyForReading()) {
    loadChars();
  }

  return mBuffer.size();
}

char LineBuffer::getChar() {
  if (mBuffer.empty()) {
    loadChars();
  }
  char ch = mBuffer.front();
  mBuffer.pop_front();
  if (ch == '\n') {
    --mLinesReady;
  }
  return ch;
}

size_t LineBuffer::linesReady() {
  if (!isInitialized()) {
    LOGW("linesReady called on unitialized LineBuffer");
    return 0;
  }

  while (mLinesReady == 0 && mFd->isReadyForReading()) {
    loadChars();
  }

  return mLinesReady;
}

std::string LineBuffer::getLine() {
  while (mLinesReady == 0) {
    loadChars();
  }
  auto it = mBuffer.begin();
  while (*it != '\n') {
    ++it;
  }
  std::string line(mBuffer.begin(), it);
  ++it;
  mBuffer.erase(mBuffer.begin(), it);
  --mLinesReady;
  return line;
}

void LineBuffer::loadChars() {
  if (mFd == nullptr) {
    throw std::runtime_error("Cannot read from an unitialized LineBuffer");
  }
  for (char ch : mFd->read()) {
    mBuffer.push_back(ch);
    if (ch == '\n') {
      ++mLinesReady;
    }
  }
}

void swap(LineBuffer& buffer1, LineBuffer& buffer2) {
  buffer1.swapWith(buffer2);
}

}}
