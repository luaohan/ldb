bench: $(wildcard *.cc) ../net/socket.cc ../dbclient/ldbc.cc ../util/protocol.cc ../dbclient/internal/client_impl.cc
	g++  $^ -o $@ -I../ -I../dbclient -I/home/wangpeng/jsoncpp-src-0.5.0/include /usr/include/jsoncpp/libjson.a

.PHONY: clean

clean:
	rm bench

