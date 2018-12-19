CCX=g++

CXXFLAGS= -g

TARGET= handgesture

OPENCV= `pkg-config --cflags opencv` `pkg-config --libs opencv`

OBJS = *.cpp
all: ${OBJS}
	$(CCX) $(CXXFLAGS)   -o ${TARGET} ${OBJS} $(OPENCV)

####### Alias for cleaning and running
.PHONY: run clean

run : 
	./$(TARGET)

clean:
	rm -f $(OBJECT) $(BIN)/$(TARGET)