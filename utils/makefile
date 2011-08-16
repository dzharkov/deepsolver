

CXXFLAGS=-O2 -g
FLAGS=-Wall -pedantic
HEADERS=Package.h
OBJS=rpm-access.o Package.o

all: rpm-access

clean:
	/bin/rm -f rpm-access *.o

Package.o: Package.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) Package.cpp

rpm-access.o: rpm-access.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) rpm-access.cpp

rpm-access: $(OBJS)
	g++ -orpm-access $(CXXFLAGS) $(FLAGS) $(OBJS) -lrpm
