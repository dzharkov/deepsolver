

CXXFLAGS=-O2 -g
FLAGS=-Wall -pedantic
HEADERS=Package.h SATBase.h
OBJS=Package.o SATFactory.o

all: rpm-access sat-test

clean:
	/bin/rm -f rpm-access sat-test *.o

Package.o: Package.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) Package.cpp

SATFactory.o: SATFactory.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) SATFactory.cpp

rpm-access.o: rpm-access.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) rpm-access.cpp

sat-test.o: sat-test.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) sat-test.cpp

rpm-access: rpm-access.o $(OBJS)
	g++ -orpm-access $(CXXFLAGS) $(FLAGS) rpm-access.o $(OBJS) -lrpm

sat-test: sat-test.o $(OBJS)
	g++ -osat-test $(CXXFLAGS) $(FLAGS) sat-test.o $(OBJS) -lrpm
