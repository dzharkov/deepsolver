

CXXFLAGS=-O2 -g
FLAGS=-Wall -pedantic

all: rpm-access

clean:
	/bin/rm -f rpm-access *.o

rpm-access.o: rpm-access.cpp
	g++ -c $(CXXFLAGS) $(FLAGS) rpm-access.cpp

rpm-access: rpm-access.o
	g++ -orpm-access $(CXXFLAGS) $(FLAGS) rpm-access.o -lrpm
