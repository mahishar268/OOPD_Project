CXX = g++
CXXFLAGS = -std=c++17
LIBS = -lstdc++fs
TARGETS = Q1 Q3 Q4
SCRIPT = Q2.sh

.PHONY: all clean

all: $(TARGETS)

Q1: Q1.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)

Q3: Q3.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)

Q4: Q4.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)

clean:
	rm -f $(TARGETS)
