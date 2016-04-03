ACLOCAL_AMFLAGS = -I m4

baseincludedir = $(includedir)/MarathonKit
coreincludedir = $(includedir)/MarathonKit/Core
soundincludedir = $(includedir)/MarathonKit/Sound

lib_LIBRARIES = libMarathonKitCore.a
if SOUND_ENABLED
lib_LIBRARIES += libMarathonKitSound.a
endif

check_PROGRAMS = MarathonKitCoreTest
check_LIBRARIES = libgmock.a
TESTS = $(check_PROGRAMS)

EXTRA_DIST = LICENSE


baseinclude_HEADERS = \
	include/MarathonKit/Core.h \
	include/MarathonKit/DebugMacro.h \
	include/MarathonKit/LogMacro.h \
	include/MarathonKit/Sound.h
coreinclude_HEADERS = \
	include/MarathonKit/Core/FileDescriptor.h \
	include/MarathonKit/Core/HashGraph.h \
	include/MarathonKit/Core/LineBuffer.h \
	include/MarathonKit/Core/Log.h \
	include/MarathonKit/Core/MessageFileDescriptor.h \
	include/MarathonKit/Core/Network.h \
	include/MarathonKit/Core/StreamFileDescriptor.h \
	include/MarathonKit/Core/TcpClient.h
soundinclude_HEADERS = \
	include/MarathonKit/Sound/SoundFile.h \
	include/MarathonKit/Sound/SoundTrack.h


libMarathonKitCore_a_CPPFLAGS = \
	$(WARNINGS_CPPFLAGS) \
	-I $(srcdir)/include/MarathonKit
libMarathonKitCore_a_SOURCES = \
	src/Core/FileDescriptor.cpp \
	src/Core/LineBuffer.cpp \
	src/Core/Log.cpp \
	src/Core/MessageFileDescriptor.cpp \
	src/Core/Network.cpp \
	src/Core/StreamFileDescriptor.cpp \
	src/Core/TcpClient.cpp

libMarathonKitSound_a_CPPFLAGS = \
	$(WARNINGS_CPPFLAGS) \
	-I $(srcdir)/include/MarathonKit
libMarathonKitSound_a_CXXFLAGS = $(sndfile_CFLAGS)
libMarathonKitSound_a_SOURCES = \
	src/Sound/SoundFile.cpp \
	src/Sound/SoundTrack.cpp


MarathonKitCoreTest_CPPFLAGS = \
	$(WARNINGS_CPPFLAGS) \
	-I $(srcdir)/include/MarathonKit \
	-isystem $(srcdir)/test/mocks \
	-isystem $(srcdir)/third-party/gmock-1.7.0/fused-src
MarathonKitCoreTest_LDADD = libgmock.a libMarathonKitCore.a
MarathonKitCoreTest_SOURCES = \
	test/HashGraphTest.cpp \
	test/LineBufferTest.cpp \
	test/mocks/MockFileDescriptor.h

libgmock_a_CPPFLAGS = \
	$(GTEST_CPPFLAGS) \
	-I $(srcdir)/third-party/gmock-1.7.0/fused-src
libgmock_a_SOURCES = \
	third-party/gmock-1.7.0/fused-src/gmock-gtest-all.cc \
	third-party/gmock-1.7.0/fused-src/gmock/gmock.h \
	third-party/gmock-1.7.0/fused-src/gmock_main.cc \
	third-party/gmock-1.7.0/fused-src/gtest/gtest.h
