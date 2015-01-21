test: $(wildcard *.cc)
	g++ -g $^ -o $@ -I../../ -L../../dbclient -lldbclient -Wl,-rpath=../../dbclient

.PHONY: clean

clean:
	rm test

