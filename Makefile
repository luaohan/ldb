#teat: g++ -o test 1.cc libleveldb.a -lpthread


#SRC := $(wildcard *.cc) 

SRC := 	./dbserver/*.cc \
		./net/*.cc \
		./util/*.cc 

server: $(SRC)
	g++ -g $^ -o $@ ../opensource/leveldb/libleveldb.a -lpthread -I../opensource/leveldb/include -lsnappy

.PHONY: clean

clean: 
	-rm server
