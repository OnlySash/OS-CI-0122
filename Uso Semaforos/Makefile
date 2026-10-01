CXX = g++
CXXFLAGS = -Wall -g

prueba: main.o Semaforos.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc Semaforos.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o prueba 