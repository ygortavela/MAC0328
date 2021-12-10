SRCS=$(wildcard *.cpp)
DEPS=$(SRCS:.cpp=.d)
OBJS=$(SRCS:.cpp=.o)
PROG=main

INPUTS=$(sort $(wildcard tests/in*))
OUTS=$(INPUTS:tests/in%=tests/out%)

.SECONDARY: $(OUTS)

CXX=g++
CXXFLAGS=-Wall -pedantic -O2 -std=c++14 -I ~/boost_1_76_0
RM=rm -fv

COMPILE.c = $(CXX) $(CXXFLAGS)
OUTPUT_OPTION=-MMD -MP -c
LINK.o = $(CXX) $(LDFLAGS)

all: $(PROG)

%.o: %.cpp
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(PROG) : $(OBJS)
	$(LINK.o) $^ -o $@

.PHONY: clean run test

run: $(PROG)
	./$(PROG)

clean:
	$(RM) $(PROG) $(OBJS) $(DEPS) $(OUTS)

test: $(OUTS)

tests/out%: tests/in% $(PROG)
	-./$(PROG) < $< > $@ 2>&1

-include $(DEPS)
