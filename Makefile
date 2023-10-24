CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

SRC = workman.cpp
OBJ = $(SRC:.cpp=.o)
DEP = $(OBJ:.o=.d)

.PHONY: all clean

all: workman

workman: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEP)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	$(RM) workman $(OBJ) $(DEP)
