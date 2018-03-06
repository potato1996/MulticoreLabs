CXX=g++
CXXFLAGS=-std=c++11 -fopenmp -O2 -Wall -I.
DEPS = Common.h \
	   SerialSolution.h \
	   MultithreadSolution.h

OBJ = ptsm.o \
	  SerialSolution.o \
	  MultithreadSolution.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

ptsm: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f *.o ptsm
