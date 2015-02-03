SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)
TARGET:=config_test


CXXFLAGS := -g -O0 \
	-I../../../opensource/jsoncpp/include \
	-I../../

LDFLAGS :=  \
	-L../../../opensource/jsoncpp \
	-Wl,-rpath=../../../opensource/jsoncpp \
	-ljsoncpp \
	-L../../util \
	-lldbutil \
	-Wl,-rpath=../../util \


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

