bench: $(wildcard *.cc) ../net/socket.cc ../dbclient/ldbc.cc ../util/protocol.cc
	g++ -I../ $^ -o $@

.PHONY: clean

clean:
	rm bench

