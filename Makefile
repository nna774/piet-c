all:

CXX = g++ 
CXXFLAGS = -std=c++11 -Wall -Wextra -Weffc++ -I src/interpreter/

INTERPRETER = src/interpreter/color.hpp src/interpreter/main.cpp
PIET = src/interpreter/piet.hpp

RM = rm -f

test: nop1x1 div_by_2

nop1x1: $(INTERPRETER) test/nop1x1.hpp
	cp test/nop1x1.hpp $(PIET)
	$(CXX) $(CXXFLAGS) src/interpreter/main.cpp -o nop1x1
	./nop1x1

div_by_2: $(INTERPRETER) test/div_by_2.hpp
	cp test/div_by_2.hpp $(PIET)
	$(CXX) $(CXXFLAGS) src/interpreter/main.cpp -o div_by_2
	./div_by_2

clean:
	$(RM) $(PIET) nop1x1 div_by_2

.PHONY: test clean
