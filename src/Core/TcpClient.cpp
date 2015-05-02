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

#include "Core/Network.h"

#include "Core/TcpClient.h"

namespace MarathonKit {
namespace Core {

using std::istringstream;
using std::string;
using std::swap;

TcpClient::TcpClient():
  mFd(),
  mLineBuffer() {}

TcpClient::TcpClient(const std::string& host, const std::string& service):
  mFd(Network::createTcpConnection(host, service)),
  mLineBuffer(mFd) {}

TcpClient::TcpClient(TcpClient&& other):
  mFd(),
  mLineBuffer() {
  swapWith(other);
}

TcpClient& TcpClient::operator = (TcpClient&& other) {
  swapWith(other);
  return *this;
}

void TcpClient::swapWith(TcpClient& other) {
  swap(mFd, other.mFd);
  swap(mLineBuffer, other.mLineBuffer);
}

bool TcpClient::isConnected() const {
  return mLineBuffer.isInitialized();
}

void TcpClient::sendLine(const string& line) {
  sendRaw(line + "\n");
}

void TcpClient::sendRaw(const string& data) {
  if (!isConnected()) {
    throw std::runtime_error("send called on a disconnected TcpSocket");
  }
  mFd->write(data);
}

size_t TcpClient::charsReady() {
  return mLineBuffer.charsReady();
}

size_t TcpClient::linesReady() {
  return mLineBuffer.linesReady();
}

char TcpClient::getChar() {
  return mLineBuffer.getChar();
}

string TcpClient::getLine() {
  return mLineBuffer.getLine();
}

void swap(TcpClient& client1, TcpClient& client2) {
  client1.swapWith(client2);
}

}}
