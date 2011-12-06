
all: proba 

clean:
	/bin/rm -f *.o proba

ZLibInterface.o: ZLibInterface.cpp ZLibInterface.h
	g++ -c ZLibInterface.cpp

proba: main.cpp
	g++ -oproba main.cpp
