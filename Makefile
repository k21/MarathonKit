CUSTOM_FLAGS := -std=c++11
CUSTOM_FLAGS += -Wall -Wextra -Wshadow -Weffc++ -Wconversion
CUSTOM_FLAGS += -O2 -fno-omit-frame-pointer -g3 -ggdb
CUSTOM_FLAGS += -I include/MarathonKit

CORE_SOURCES := $(wildcard src/Core/*.cpp)
SOUND_SOURCES := $(wildcard src/Sound/*.cpp)

CORE_OBJS := $(CORE_SOURCES:src/%.cpp=build-dir/objs/%.o)
SOUND_OBJS := $(SOUND_SOURCES:src/%.cpp=build-dir/objs/%.o)

ALL := \
	libMarathonKitCore.a \
	libMarathonKitSound.a \


all: $(ALL)

libMarathonKitCore.a: $(CORE_OBJS)
	mkdir -p `dirname $@`
	$(AR) $(ARFLAGS) $@ $^

libMarathonKitSound.a: $(SOUND_OBJS)
	mkdir -p `dirname $@`
	$(AR) $(ARFLAGS) $@ $^


-include $(CORE_OBJS:.o=.d)
-include $(SOUND_OBJS:.o=.d)


build-dir/objs/%.o: src/%.cpp Makefile
	mkdir -p `dirname $@`
	$(CXX) -c -MMD -MP $(CUSTOM_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<


.PHONY: all clean

clean:
	rm -rf build-dir $(ALL)
