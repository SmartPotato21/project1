CXX = g++
CXXFLAGS = -std=c++11 -pthread -Wall -O2


all: server client

server: server.cpp
	$(CXX) $(CXXFLAGS) -o runserver server.cpp

client: client.cpp
	$(CXX) $(CXXFLAGS) -o runclient client.cpp

run_server: server
	./runserver

run_client: client
	./runclient

clean:
	rm -f server client
