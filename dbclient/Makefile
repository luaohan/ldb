SRC=$(wildcard *.cc) $(wildcard internal/*.cc)
OBJ=$(SRC:%.cc=%.o)
TARGET:=libldbclient.so


CXXFLAGS := -g -O0 -fPIC \
	-I./ \
	-I../ \
	-I../../opensource/jsoncpp/include

LDFLAGS := -L../net \
	-Wl,-rpath=../net \
	-lldbnet \
	-L../util \
	-Wl,-rpath=../util \
	-lldbutil \
	#-L../../opensource/jsoncpp/build/debug/src/lib_json/ \
	-Wl,-rpath=../../opensource/jsoncpp/build/debug/src/lib_json/ \
	-ljsoncpp
	-L../../opensource/jsoncpp/ \
	-Wl,-rpath=../../opensource/jsoncpp/ \
	-ljsoncpp

$(TARGET): $(OBJ) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $^ -o $@

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

