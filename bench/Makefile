CPPFLAGS=-mavx2 -O2 -std=c++11
SRC=bfs_cpu.cpp bfs_avx2.cpp highres.cpp
OBJ=$(SRC:.cpp=.o)

all: $(OBJ)
	g++ $(CPPFLAGS) *.o -o a.out

bfs_cpu.o: bfs_cpu.cpp

bfs_avx2.o: bfs_avx2.cpp

highres.o: highres.cpp
