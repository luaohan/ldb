SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)
TARGET:=test


CXXFLAGS := -g -O0 \
	-I../../../opensource/leveldb/include \
	-I../../../opensource/jsoncpp/include \
	-I../../

LDFLAGS := -L../../net \
	-Wl,-rpath=../../net \
	-lldbnet \
	-L../../util \
	-lldbutil \
	-Wl,-rpath=../util \
	-L../../../opensource/leveldb \
	-lleveldb \
	-Wl,-rpath=../../../opensource/leveldb \
	-L../../../opensource/jsoncpp \
	-ljsoncpp \
	-Wl,-rpath=../../../opensource/jsoncpp \
	-L../../dbclient \
	-lldbclient \
	-Wl,-rpath=../../dbclient


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

