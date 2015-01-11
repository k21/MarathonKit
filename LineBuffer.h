#ifndef MARATHON_KIT_LINE_BUFFER_H_
#define MARATHON_KIT_LINE_BUFFER_H_

#include <deque>
#include <string>

namespace MarathonKit {

class LineBuffer {
public:

  LineBuffer();

  void clear();

  void putChar(char ch);

  size_t charsReady() const;
  char getChar();

  size_t linesReady() const;
  std::string getLine();

private:

  std::deque<char> mBuffer;
  std::size_t mLinesReady;

};

}

#endif
