LIBS=-lssl -lcrypto -L./thirdparty4linux -L./ -lpthread -ldl -lrt 

CXXFLAGS = -g -Wall -fPIC

CXX := g++

all: client_demo server_demo

# build
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< 

# link
client_demo : client_demo.o hyc_ssl_contex.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

server_demo: server_demo.o hyc_ssl_contex.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -f server_demo client_demo *.o 
