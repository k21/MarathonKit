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

#ifndef MARATHON_KIT_LOG_H_
#define MARATHON_KIT_LOG_H_

#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace MarathonKit {

class Log {
public:

  enum class Level {
    DEBUG,
    INFO,
    WARN,
    ERR,
  };

  Log(const std::string& file = "", int line = 0):
    mFile(file),
    mLine(line) {}

  template <typename... Types>
  void d(const Types&... objects) {
    log(Level::DEBUG, objects...);
  }

  template <typename... Types>
  void i(const Types&... objects) {
    log(Level::INFO, objects...);
  }

  template <typename... Types>
  void w(const Types&... objects) {
    log(Level::WARN, objects...);
  }

  template <typename... Types>
  void e(const Types&... objects) {
    log(Level::ERR, objects...);
  }

  template <typename... Types>
  void log(Level level, Types... objects) {
    if (level < getMinLogLevel()) {
      return;
    }

    time_t epoch = time(nullptr);
    struct tm* local = localtime(&epoch);
    char timeStr[256];
    if (strftime(timeStr, 256, "%Y-%m-%d %H:%M:%S", local) == 0) {
      strcpy(timeStr, "?");
    }

    std::ostringstream outputStream;
    outputStream << timeStr
        << ' '
        << std::setw(5)
        << mLevelString[static_cast<int>(level)]
        << ' ';

    if (!mFile.empty()) {
      outputStream << mFile;
    } else {
      outputStream << '?';
    }

    outputStream << ':';

    if (mLine > 0) {
      outputStream << mLine;
    } else {
      outputStream << '?';
    }

    outputStream << '\t';
    writeObjectsToStream(outputStream, objects...);
    outputStream << '\n';

    std::cerr << outputStream.str();
    std::cerr.flush();

    if (mLogFile.is_open()) {
      mLogFile << outputStream.str();
      mLogFile.flush();
    }
  }

  static void setMinLogLevel(Level level) { mMinLogLevel = level; }
  static Level getMinLogLevel() { return mMinLogLevel; }
  static void setLogFile(const std::string& fileName);

private:

  template <typename Type, typename... Types>
  static void writeObjectsToStream(
      std::ostream& outputStream,
      const Type& object,
      const Types&... objects) {
    outputStream << object;
    writeObjectsToStream(outputStream, objects...);
  }

  static void writeObjectsToStream(std::ostream&) {}

  std::string mFile;
  int mLine;

  static Level mMinLogLevel;
  static char mLevelString[4][10];
  static std::ofstream mLogFile;

};

}

#endif
