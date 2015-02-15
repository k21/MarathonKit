MarathonKit
===========

MarathonKit is a library that simplifies writing programs during competitions
like Challenge24, Marathon24 or Deadline24. MarathonKit aims to speed up certain
tasks that commonly occur during those competitions. All of those competitions
let contestants use code that was written before the competition and often
require them to communicate with a remote server over a network. Inside the
library you can find code that simplifies network communication and other
functionality that might be useful.

Installation
------------

After cloning the repository, run `make` to build a static library.
If everything goes smoothly, this should create a `libMarathonKit.a` file in the
root of the repository that you can link with your other sources. To use
the library in your program, set the include path to `$MARATHON_KIT_DIR/include`
(where `$MARATHON_KIT_DIR` is the path to the root of the MarathonKit
repository) and then include its header files using for example
`#include <MarathonKit/Core.h>`.

Features
--------

### Network communication ###

You can use the `MarathonKit::Network` class to create TCP and UDP connections
easily. To create a TCP client, create an instance of the class `TcpClient`. It
takes the host name and service port of the remote server as parameters of its
constructor. For example the following code will connect to a local server on
port 1234, send a single line and then prints the first line it receives back:

```c++
TcpClient tcp("localhost", "1234");
tcp.sendLine("Hello world!");
std::cout << tcp.getLine() << std::endl;
```

To create an UDP listener, use the function `Network::createUdpListener`. It
takes the service port on which you want to listen as its parameter and returns
an instance of a class `FileDescriptor` that you can use to read the incoming
messages. Here is an example of a code that will listen on port 1234 and print
all incoming messages:

```c++
FileDescriptor udp = Network::createUdpListener("1234");
while (true) {
  std::cout << udp.read() << std::endl;
}
```

### Logging and debugging ###

If you include the `MarathonKit/LogMacro.h` header file, you can use the macros
`LOGD`, `LOGI`, `LOGW` and `LOGE` to log debugging information, information,
warnings and errors to standard error. All of those macros take an arbitrary
number of parameters an convert them to strings using the C++ stream operators.
For example:

```c++
std::string fileName = "a.txt";
size_t fileSize = 1234;
LOGD("File ", fileName, " is ", fileSize, " bytes long.");
```

This will print out `File a.txt is 1234 bytes long.`

You can also call the static function `MarathonKit::Log::setLogFile(fileName)`
to send the log into a file as well or `MarathonKit::Log::setMinLogLevel(level)`
to set the minimum logging level.

In addition to those, you can use the macro `DEBUG` defined in
`MarathonKit/DebugMacro.h` to quickly print out the contents of variables:

```c++
int x = 42;
std::string s = "foo";
DEBUG(x, s);
```

This code will print out `(x, s) = (42, foo)`.

License
-------

This file is part of MarathonKit.
Copyright (C) 2015 Jakub Zika

MarathonKit is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

I am providing code in this repository to you under an open source license.
Because this is my personal repository, the license you receive to my code is
from me and not from my employer (Facebook).
