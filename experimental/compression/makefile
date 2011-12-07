
all: proba 

clean:
	/bin/rm -f *.o proba

main.o: main.cpp
	g++ -c main.cpp

proba: main.o
	g++ -oproba main.o -lz
