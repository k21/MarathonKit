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
