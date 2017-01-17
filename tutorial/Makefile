VOYAGER_DIRECTORY = ./build/release-install
VOYAGER_INCLUDE = $(VOYAGER_DIRECTORY)/include
VOYAGER_LIBRARY = $(VOYAGER_DIRECTORY)/lib

#GPERFTOOLS = /usr/local

CXXFLAGS = -std=c++11 -g -O2 -Wall -Wextra -Werror \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic \
	   -I$(VOYAGER_INCLUDE)    \

LDFLAGS = -L$(VOYAGER_LIBRARY) -lvoyager_core -lvoyager_port  \
		  -lvoyager_util -lpthread \
#		  -L$(GPERFTOOLS)/lib -lprofiler

TESTS = echo

all: $(TESTS)

$(TESTS):
	g++ $(CXXFLAGS) -I./ -o $@ $(filter %.cc, $^) $(LDFLAGS)
#	g++ $(CXXFLAGS) -I./ -I$(GPERFTOOLS)/include -o $@ $(filter %.cc, $^) $(LDFLAGS)

echo: echo.cc

clean:
	rm -f $(TESTS)

.PHONY: all clean
