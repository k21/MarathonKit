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

using std::unique_ptr;

LineBuffer::LineBuffer():
  mReader(),
  mBuffer(),
  mLinesReady(0) {}

LineBuffer::LineBuffer(unique_ptr<IReader>&& reader):
  mReader(std::move(reader)),
  mBuffer(),
  mLinesReady(0) {}

bool LineBuffer::isInitialized() const {
  return mReader != nullptr;
}

size_t LineBuffer::charsReady() const {
  return mBuffer.size();
}

char LineBuffer::getChar() {
  if (mBuffer.empty()) {
    loadChar();
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
  while (mLinesReady == 0) {
    loadChar();
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

void LineBuffer::loadChar() {
  if (mReader == nullptr) {
    throw std::runtime_error("Cannot read from an unitialized LineBuffer");
  }
  char ch = mReader->getChar();
  mBuffer.push_back(ch);
  if (ch == '\n') {
    ++mLinesReady;
  }
}

}
