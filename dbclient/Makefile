SRC := $(wildcard *.cc) $(wildcard internal/*.cc) ../net/socket.cc ../util/protocol.cc

libldbc.so: $(SRC)
	        g++ -g $^ -shared -fPIC -o $@ -I./ -I../

.PHONY: clean

clean:
	-rm libldbc.so
