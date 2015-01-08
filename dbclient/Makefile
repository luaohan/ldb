#teat: g++ -o test 1.cc libleveldb.a -lpthread


#SRC := $(wildcard *.cc) 
SRC := ../net/socket.cc \
	./ldbc.cc \
	./test.cc \
	../util/protocol.cc

client: $(SRC)
	        g++ -g $^ -o $@ 

.PHONY: clean

clean: 
	        -rm client 
