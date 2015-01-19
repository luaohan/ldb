all: util net dbserver dbclient
	@echo "make complete"

util:
	make -C util

net: util
	make -C net

dbserver: util net
	make -C dbserver

dbclient: util net
	make -C dbclient

.PHONY: clean util

clean: 
	make clean -C dbserver
	make clean -C dbclient
	make clean -C net
	make clean -C util 
