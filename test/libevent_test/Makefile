SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)
TARGET:=test


CXXFLAGS := -g -O0 \
	-I../../

LDFLAGS := -L../../net \
	-Wl,-rpath=../../net \
	-lldbnet \
	-L/usr/local/lib \
	-Wl,-rpath=/usr/local/lib \
	-levent \


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

