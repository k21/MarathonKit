CUSTOM_FLAGS := -std=c++11
CUSTOM_FLAGS += -Wall -Wextra -Wshadow -Weffc++ -Wconversion
CUSTOM_FLAGS += -O2 -fno-omit-frame-pointer -g3 -ggdb
CUSTOM_FLAGS += -I include/MarathonKit

SOURCES := $(wildcard src/*.cpp)

OBJS := $(SOURCES:src/%.cpp=build-dir/objs/%.o)


all: libMarathonKit.a

libMarathonKit.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

-include $(OBJS:.o=.d)

build-dir/objs/%.o: src/%.cpp Makefile | build-dir/objs
	$(CXX) -c -MMD -MP $(CUSTOM_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<


build-dir/objs: | build-dir
	mkdir build-dir/objs

build-dir:
	mkdir build-dir


.PHONY: all clean

clean:
	rm -rf build-dir libMarathonKit.a
