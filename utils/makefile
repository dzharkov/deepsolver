

CXXFLAGS=-O2 -g

all: rpm-access

clean:
	/bin/rm -f rpm-access *.o

rpm-access.o: rpm-access.cpp
	g++ -c $(CXXFLAGS) rpm-access.cpp

rpm-access: rpm-access.o
	g++ -orpm-access $(CXXFLAGS) rpm-access.o -lrpm
