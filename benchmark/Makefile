bench: $(wildcard *.cc) ../net/socket.cc ../dbclient/ldbc.cc ../util/protocol.cc ../dbclient/internal/client_impl.cc
	g++ -I../ -I../dbclient $^ -o $@

.PHONY: clean

clean:
	rm bench

