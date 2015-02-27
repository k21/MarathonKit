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

#ifndef MARATHON_KIT_CORE_LINE_BUFFER_H_
#define MARATHON_KIT_CORE_LINE_BUFFER_H_

#include <deque>
#include <memory>
#include <string>

namespace MarathonKit {
namespace Core {

class FileDescriptor;

class LineBuffer {
public:

  LineBuffer();
  explicit LineBuffer(const std::shared_ptr<FileDescriptor>& fd);

  bool isInitialized() const;

  size_t charsReady();
  char getChar();

  size_t linesReady();
  std::string getLine();

private:

  void loadChars();

  std::shared_ptr<FileDescriptor> mFd;
  std::deque<char> mBuffer;
  std::size_t mLinesReady;

};

}}

#endif
