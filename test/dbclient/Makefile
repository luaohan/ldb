test: $(wildcard *.cc)
	g++ -g $^ -o $@ -I../../ -L../../dbclient -lldbc -Wl,-rpath=../../dbclient

.PHONY: clean

clean:
	rm test

