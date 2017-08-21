#ifndef BFS_AVX2

// this uses AVX2, so only supports Haswell later

#include <iostream>
#include <fstream>
#include <chrono>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <cstdlib>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>

#include "bfs.h"

/*
  instance should be singleton
 */

class bfs_avx2 : public bfs {
 public:
  int load_graph(std::vector<int>& vertices,  std::vector<std::pair<int,int>>& graph );
  int done();
  int reset();
  int retry();
  int distance_alloc_count();
  int distance_alloc_size();
  int distance_count();
  int traverse(int s, int *d);
};

#endif

