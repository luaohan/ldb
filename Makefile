#teat: g++ -o test 1.cc libleveldb.a -lpthread


#SRC := $(wildcard *.cc) 

SRC := 	./dbserver/*.cc \
		./net/*.cc \
		./util/*.cc 

server: $(SRC)
	g++ -g $^ -o $@ libleveldb.a -lpthread

.PHONY: clean

clean: 
	-rm server
