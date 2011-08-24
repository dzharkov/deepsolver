

CXXFLAGS=-O2 -g
FLAGS=-Wall -pedantic
HEADERS=Package.h SATBase.h VerCmp.h
OBJS=Package.o SATFactory.o VerCmp.o

all: rpm-access sat-test provide-filter

clean:
	/bin/rm -f rpm-access sat-test provide-filter *.o

Package.o: Package.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) Package.cpp

SATFactory.o: SATFactory.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) SATFactory.cpp

VerCmp.o: VerCmp.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) VerCmp.cpp

rpm-access.o: rpm-access.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) rpm-access.cpp

sat-test.o: sat-test.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) sat-test.cpp

provide-filter.o: provide-filter.cpp $(HEADERS)
	g++ -c $(CXXFLAGS) $(FLAGS) provide-filter.cpp

rpm-access: rpm-access.o $(OBJS)
	g++ -orpm-access $(CXXFLAGS) $(FLAGS) rpm-access.o $(OBJS) -lrpm

sat-test: sat-test.o $(OBJS)
	g++ -osat-test $(CXXFLAGS) $(FLAGS) sat-test.o $(OBJS) -lrpm -lrpmdb

provide-filter: provide-filter.o $(OBJS)
	g++ -oprovide-filter $(CXXFLAGS) $(FLAGS) provide-filter.o $(OBJS) -lrpm -lrpmdb
