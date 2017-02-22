CXXFLAGS = -std=c++11 -g -O2 -Wall -Wextra -Werror \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic \
	   -I/usr/local/include    \

LDFLAGS = -L/usr/local/lib -lvoyager_core -lvoyager_port  \
	  -lvoyager_util -lpthread \
#         -lprofiler

TESTS = echo

all: $(TESTS)

$(TESTS):
	g++ $(CXXFLAGS) -I./ -o $@ $(filter %.cc, $^) $(LDFLAGS)

echo: echo.cc

clean:
	rm -f $(TESTS)

.PHONY: all clean
