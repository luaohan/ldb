SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)
TARGET:=dbserver


CXXFLAGS := -g -O0 -I../../opensource/leveldb/include -I../

LDFLAGS := -L../net -Wl,-rpath=../net -lldbnet \
	-L../util -Wl,-rpath=../util -lldbutil \
	-L../../opensource/leveldb -Wl,-rpath=../../opensource/leveldb -lleveldb \
	-L../../opensource/libevent -Wl,-rpath=../../opensource/libevent -levent

$(TARGET): $(OBJ) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) $< -c -o $@

%.d: %.cc
	@$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

sinclude $(SRC:.cc=.d)

.PHONY: clean

clean:
	@rm $(TARGET) $(OBJ) *.d -f

