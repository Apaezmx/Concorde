CXXFLAGS=-std=c++11 -fpic

libconcorde.so: concorde.o
	g++ -shared concorde.o -o libconcorde.so

concorde.o: concorde.cc
	g++ -fpic -c -std=c++11 concorde.cc
