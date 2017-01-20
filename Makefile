
CXXFLAGS = -c -std=c++11 -I/usr/local/opt/llvm/include/

LFLAGS= -L/usr/local/opt/llvm/lib/ -lomp

all: out

clean:
	-$(RM) *.o out

out: main.o
	g++ -o $@ -O3 -std=c++11 main.o $(LFLAGS)

test: test.o
	g++ -o $@ -O3 test.o $(LFLAGS)

%.o:%.c
	g++ -o $@ $(CXXFLAGS) $^