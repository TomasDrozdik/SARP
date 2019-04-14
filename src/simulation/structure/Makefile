CC=g++
C_STD=c++17
CPPFLAGS=-Wall -std=$(C_STD) -pedantic -Wpointer-arith -Wcast-qual -g
OBJECTS= $(patsubst %.cc, %.o, $(wildcard *.cc))

all: $(OBJECTS)

%.o: %.cc %.h
	$(CC) $(CPPFLAGS) -c $<

clean:
	rm -rf $(OBJECTS)


.PHONY: all clean
