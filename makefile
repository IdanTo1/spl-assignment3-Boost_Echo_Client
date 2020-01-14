CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -pthread -L/usr/lib

# All Targets
all: StompBookClubClient

# Linker
StompBookClubClient: bin/StompBookClubClient.o
	g++ -o bin/StompBookClubClient.o bin/ConnectionHandler.o bin/UserHandler.o bin/ServerHandler \
		bin/ConcurrentDataQueues.o bin/Frame.o bin/ClientInventory.o $(LDFLAGS)

# Source Files
bin/StompBookClubClient.o: src/StompBookClubClient.cpp include/StompBookClubClient.h \
		bin/ConnectionHandler.o bin/UserHandler.o bin/ServerHandler
	g++ $(CFLAGS) -o bin/StompBookClubClient.o src/StompBookClubClient.cpp

bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/ServerHandler.o: src/ServerHandler.cpp include/ServerHandler.h bin/ConcurrentDataQueues.o bin/Frame.o bin/ClientInventory.o
	g++ $(CFLAGS) -o bin/ServerHandler.o src/ServerHandler.cpp

bin/UserHandler.o: src/UserHandler.cpp include/UserHandler.h bin/ConcurrentDataQueues.o bin/Frame.o bin/ClientInventory.o
	g++ $(CFLAGS) -o bin/UserHandler.o src/UserHandler.cpp

bin/ConcurrentDataQueues.o: src/ConcurrentDataQueues.cpp include/ConcurrentDataQueues.h bin/Frame.o
	g++ $(CFLAGS) -o bin/ConcurrentDataQueues.o src/ConcurrentDataQueues.cpp

bin/Frame.o: src/Frame.cpp include/Frame.h
	g++ $(CFLAGS) -o bin/Frame.o src/Frame.cpp

bin/ClientInventory.o: src/ClientInventory.cpp include/ClientInventory.h
	g++ $(CFLAGS) -o bin/ClientInventory.o src/ClientInventory.cpp

.PHONY: clean, all
# clean the build directory.
clean:
	rm -f bin/*
