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

#include <cerrno>
#include <cstring>
#include <unistd.h>

#include <stdexcept>

#include "Pipe.h"

namespace MarathonKit {

Pipe::Pipe():
  mReadFd(),
  mWriteFd() {
  int fildes[2];
  int rc = pipe(fildes);
  if (rc != 0) {
    throw std::runtime_error(std::strerror(errno));
  }
  mReadFd = FileDescriptor::createOwnerOf(fildes[0]);
  mWriteFd = FileDescriptor::createOwnerOf(fildes[1]);
}

}
