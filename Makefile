COMMON_FLAGS := -std=c++11
COMMON_FLAGS += -pthread
COMMON_FLAGS += -O2 -fno-omit-frame-pointer -g3 -ggdb

CUSTOM_FLAGS := $(COMMON_FLAGS)
CUSTOM_FLAGS += -Wall -Wextra -Wshadow -Weffc++ -Wconversion
CUSTOM_FLAGS += -I include/MarathonKit

CORE_SOURCES := $(wildcard src/Core/*.cpp)
SOUND_SOURCES := $(wildcard src/Sound/*.cpp)

CORE_OBJS := $(CORE_SOURCES:src/%.cpp=build-dir/objs/%.o)
SOUND_OBJS := $(SOUND_SOURCES:src/%.cpp=build-dir/objs/%.o)

GMOCK_VERSION := 1.7.0
GMOCK_DIR := third-party/gmock-$(GMOCK_VERSION)/fused-src
GMOCK_FLAGS := $(COMMON_FLAGS) -I $(GMOCK_DIR)
GMOCK_SOURCES := $(GMOCK_DIR)/gmock-gtest-all.cc $(GMOCK_DIR)/gmock_main.cc
GMOCK_OBJS := $(GMOCK_SOURCES:$(GMOCK_DIR)/%.cc=build-dir/gmock-objs/%.o)

TEST_FLAGS := $(CUSTOM_FLAGS) -isystem $(GMOCK_DIR) -isystem test/mocks
TEST_LINK_FLAGS := -lsndfile
TEST_SOURCES := $(wildcard test/*.cpp)
TEST_OBJS := $(TEST_SOURCES:test/%.cpp=build-dir/test-objs/%.o)

ALL := \
	libMarathonKitCore.a \
	libMarathonKitSound.a \


all: $(ALL)

check: MarathonKitTest
	./MarathonKitTest --gtest_shuffle --gtest_color=yes

libMarathonKitCore.a: $(CORE_OBJS)
	mkdir -p `dirname $@`
	$(AR) $(ARFLAGS) $@ $^

libMarathonKitSound.a: $(SOUND_OBJS)
	mkdir -p `dirname $@`
	$(AR) $(ARFLAGS) $@ $^

MarathonKitTest: $(CORE_OBJS) $(SOUND_OBJS) $(TEST_OBJS) $(GMOCK_OBJS)
	$(CXX) $(CUSTOM_FLAGS) $(TEST_LINK_FLAGS) $(LDFLAGS) -o $@ $^


-include $(CORE_OBJS:.o=.d)
-include $(SOUND_OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)
-include $(GMOCK_OBJS:.o=.d)


build-dir/objs/%.o: src/%.cpp Makefile
	mkdir -p `dirname $@`
	$(CXX) -c -MD -MP $(CUSTOM_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

build-dir/test-objs/%.o: test/%.cpp Makefile
	mkdir -p `dirname $@`
	$(CXX) -c -MD -MP $(TEST_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

build-dir/gmock-objs/%.o: $(GMOCK_DIR)/%.cc Makefile
	mkdir -p `dirname $@`
	$(CXX) -c -MD -MP $(GMOCK_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<


.PHONY: all clean check

clean:
	rm -rf build-dir $(ALL) MarathonKitTest
