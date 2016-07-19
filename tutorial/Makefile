BUILD_DIR = ../../
VOYAGER_DIRECTORY = $(BUILD_DIR)/build/release-install
VOYAGER_INCLUDE = $(VOYAGER_DIRECTORY)/include
VOYAGER_LIBRARY = $(VOYAGER_DIRECTORY)/lib

CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -Werror \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic \
	   -I$(VOYAGER_INCLUDE)

LDFLAGS = -L$(VOYAGER_LIBRARY) -lvoyager_core -lvoyager_port -lvoyager_util -lpthread 

all: echo
clean:
	rm -f echo core

echo: echo.cc
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
