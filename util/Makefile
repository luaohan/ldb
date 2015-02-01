SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)
TARGET:=libldbutil.so


CXXFLAGS := -g -O0 -fPIC

CXXFLAGS := -g -O0 -fPIC\
	-I../../opensource/jsoncpp/include \
	-I../../

LDFLAGS :=  \
	-L../../opensource/jsoncpp \
	-Wl,-rpath=../../opensource/jsoncpp \
	-ljsoncpp \

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

