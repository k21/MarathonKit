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

#ifndef MARATHON_KIT_DEBUG_MACRO_H_
#define MARATHON_KIT_DEBUG_MACRO_H_

#include "Log.h"

#define DEBUG(...) ::MarathonKit::DebugMacroImpl::debug( \
    __FILE__, \
    __LINE__, \
    #__VA_ARGS__, \
    __VA_ARGS__);

namespace MarathonKit {

class DebugMacroImpl {
public:

  DebugMacroImpl() = delete;

  template <typename... Types>
  static void debug(
      const std::string& file,
      int line,
      const std::string& names,
      const Types&... objects) {
    std::ostringstream oss;
    oss << "(" << names << ") = (";
    writeObjectsToStream(oss, objects...);
    oss << ")";
    Log(file, line).d(oss.str());
  }

private:

  template <typename Type>
  static void writeObjectsToStream(
      std::ostringstream& oss,
      const Type& object) {
    oss << object;
  }

  template <typename Type1, typename Type2, typename... Types>
  static void writeObjectsToStream(
      std::ostringstream& oss,
      const Type1& object1,
      const Type2& object2,
      const Types&... objects) {
    oss << object1 << ", ";
    writeObjectsToStream(oss, object2, objects...);
  }

};

}

#endif
