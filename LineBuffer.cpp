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

#include "LineBuffer.h"

#include <stdexcept>

namespace MarathonKit {

LineBuffer::LineBuffer():
  mBuffer(),
  mLinesReady(0) {}

void LineBuffer::clear() {
  mBuffer.clear();
  mLinesReady = 0;
}

void LineBuffer::putChar(char ch) {
  mBuffer.push_back(ch);
  if (ch == '\n') {
    ++mLinesReady;
  }
}

size_t LineBuffer::charsReady() const {
  return mBuffer.size();
}

char LineBuffer::getChar() {
  if (mBuffer.empty()) {
    throw std::runtime_error("getChar called on an empty buffer");
  }
  char ch = mBuffer.front();
  mBuffer.pop_front();
  if (ch == '\n') {
    --mLinesReady;
  }
  return ch;
}

size_t LineBuffer::linesReady() const {
  return mLinesReady;
}

std::string LineBuffer::getLine() {
  if (mLinesReady == 0) {
    throw std::runtime_error("getLine callen on a buffer with no lines");
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

}
