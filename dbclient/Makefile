SRC := $(wildcard *.cc) $(wildcard internal/*.cc) ../net/socket.cc ../util/protocol.cc

client: $(SRC)
	        g++ -g $^ -shared -fPIC -o libldbc.so -I./ -I../

.PHONY: clean

clean:
	-rm client 
