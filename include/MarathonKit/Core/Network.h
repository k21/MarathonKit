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

#ifndef MARATHON_KIT_CORE_NETWORK_H_
#define MARATHON_KIT_CORE_NETWORK_H_

#include <memory>
#include <string>

#include "StreamFileDescriptor.h"
#include "MessageFileDescriptor.h"

namespace MarathonKit {
namespace Core {

class Network {
public:

  // Utitlity class, do not instantiate.
  Network() = delete;

  enum class Family {
    ANY,
    IP_V4,
    IP_V6,
  };

  enum class Protocol {
    TCP,
    UDP,
  };

  enum class Mode {
    ACTIVE,
    PASSIVE,
  };

  static std::unique_ptr<StreamFileDescriptor> createTcpConnection(
      const std::string& host,
      const std::string& service);

  static std::unique_ptr<MessageFileDescriptor> createUdpListener(
      const std::string& service);

};

}}

#endif
