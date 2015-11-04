LIBS=-lboost_system -lboost_filesystem -lssl -lcrypto -L./third -L./ -lpthread -ldl -lrt 

CXXFLAGS = -g -Wall -fPIC

CXX := g++

all: client server

# build
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< 

# link
client : client.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

server: server.o 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -f server client *.o 
