#ifndef MARATHON_KIT_TCP_CLIENT_H_
#define MARATHON_KIT_TCP_CLIENT_H_

#include <sstream>
#include <string>

#include "AsyncReader.h"

namespace MarathonKit {

class TcpClient {
public:

  TcpClient();
  ~TcpClient();

  void connect(const std::string& host, const std::string& service);
  void disconnect();
  bool isConnected() const;

  void sendLine(const std::string& line);
  void sendRaw(const std::string& data);

  size_t charsReady();
  size_t linesReady();

  char getChar();
  std::string getLine();
  std::istringstream getLineStream();

private:

  TcpClient(const TcpClient&) = delete;
  TcpClient& operator = (const TcpClient&) = delete;

  FileDescriptor mFd;
  AsyncReader mAsyncReader;

};

}

#endif
