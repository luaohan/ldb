CSRC=$(wildcard *.c)
OBJ:=$(SRC:%.cc=%.o)
OBJ+=$(CSRC:%.c=%.o)

TARGET=http_server

CC := gcc

CFLAGS := -g -O0 -I./
LDFLAGS :=

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

%.d: %.c
	@$(CXX) -MM $(CFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

sinclude $(CSRC:.c=.d)

.PHONY: clean

clean:
	@rm $(TARGET) $(OBJ) *.d -f

