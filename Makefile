CXXFLAGS= -I. -I./include
CXXFLAGS+=-g -Wall
CXXFLAGS+= -O2

HEADERS=$(wildcard *.h)
TESTS= logging_test                 

all: $(TESTS)

$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cc, $^) $(LDFLAGS) -lpthread

logging_test: test/logging_test.cc util/logging.cc util/status.cc \
              util/timestamp.cc 

logging_test: LDFLAGS += -std=c++0x

.PHONY: clean
clean:
	rm -f $(TESTS)
