#ifndef BFSCPU_H
#define BFSCPU_H

#include "bfs.h"

struct graphbfs;

class bfs_cpu : public bfs {
  struct graphbfs *vertex;
  int n;
 public:
  int load_graph(std::vector<int>& vertex,  std::vector<std::pair<int,int> >& graph );
  int done() ;
  int retry() ;
  int distance_alloc_count();
  int distance_alloc_size();
  int distance_count();
  int traverse(int s, int *d);
};


#endif
