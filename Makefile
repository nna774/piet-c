all:

CXX ?= g++ 
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Weffc++ -I src/interpreter/ -O2

INTERPRETER = src/interpreter/color.hpp src/interpreter/main.cpp
PIET = src/interpreter/piet.hpp

TESTS = nop1x1.test div_by_2.test

.SUFFIXES: .test

RM = rm -f

test: $(TESTS) $(TESTS:.test=.test.sh)

%.test: tests/%.hpp $(INTERPRETER)
	cp $< $(PIET)
	$(CXX) $(CXXFLAGS) src/interpreter/main.cpp -o $@

%.test.sh: tests/%.test.sh
	./tests/$@

clean:
	$(RM) $(PIET) $(TESTS)

.PHONY: test clean
