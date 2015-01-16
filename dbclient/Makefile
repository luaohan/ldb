SRC := $(wildcard *.cc) $(wildcard internal/*.cc) ../net/socket.cc ../util/protocol.cc

libldbc.so: $(SRC)
	g++ -g $^ -shared -fPIC -o $@ -I./ -I../ -I/home/wangpeng/jsoncpp-src-0.5.0/include /usr/include/jsoncpp/libjson.a

.PHONY: clean

clean:
	-rm libldbc.so
