
all: proba 

clean:
	/bin/rm -f *.o proba

ZLibInterface.o: ZLibInterface.cpp ZLibInterface.h
	g++ -c ZLibInterface.cpp

main.o: main.cpp
	g++ -c main.cpp

proba: main.o ZLibInterface.o
	g++ -oproba main.o ZLibInterface.o -lz
