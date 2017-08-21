#ifndef BFS_H
#define BFS_H
#include <cstdlib>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>
#include <vector>

struct bfs;

struct bfs {
  virtual int load_graph(std::vector<int> &vertices,  std::vector<std::pair<int,int> > &graph ) = 0;
  virtual int done() = 0;
  virtual int retry()  = 0;
  virtual int distance_alloc_count() = 0;
  virtual int distance_alloc_size() = 0;
  virtual int distance_count() = 0;
  virtual int traverse(int s = 0, int *d = 0)  = 0;
};

#endif
