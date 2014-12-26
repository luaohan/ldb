#teat: g++ -o test 1.cc libleveldb.a -lpthread


SRC := $(wildcard *.cc) 

client: $(SRC)
	        g++ -g $^ -o $@ 

.PHONY: clean

clean: 
	        -rm client 
