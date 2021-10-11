CXX=g++
CXXFLAGS=-Wall -Wextra -pedantic -Werror -std=c++20 -O3
# debug flags = -Wall -Wextra -pedantic -Werror -std=c++20 -O0 -g
LDFLAGS=$(CXXFLAGS)
OBJ=$(SRC:.cc=.o)

all: encoder decoder

encoder: encoder.o bitio.o lzw.o
	$(CXX) $(LDFLAGS) -o $@ $^

decoder: decoder.o bitio.o lzw.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf *.o encoder decoder