CXXFLAGS=-std=c++17 -fpic

libconcorde.so: concorde.o
	g++ -shared concorde.o -o libconcorde.so

concorde.o: concorde.cc
	g++ -fpic -c -std=c++17 concorde.cc
