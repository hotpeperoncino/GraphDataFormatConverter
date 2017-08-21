#include "bfs_avx2.h"

//
// https://github.com/mstelmas/BFS_SSE/blob/master/bfs_sse.cpp
// 

#if defined(__GNUC__)
static void *_aligned_malloc(std::size_t size, std::size_t alignment)
{
  void *p;
  posix_memalign(&p, alignment, size);
  return p;
}
static void _aligned_free(void *p)
{
  std::free(p);
}

#endif

using namespace std;


/* bfs avx2 */
static bool **adj;
static unsigned NN, QQ;
static unsigned roundedNN;
static bool* visited;
static bool* negvisited;
static int *distance_;
static bool *Y;
static bool *X_;
static bool *Z;


static inline __m128i XMMLOAD(void const *ptr) {
  return _mm_loadu_si128((__m128i const *)ptr);
}

static inline __m128i XMMLOAD_ALIGNED(void const *ptr) {
  return _mm_load_si128((__m128i const *)ptr);
}

static inline __m128i _mm_setone_epi8(void) {
  __m128i x = _mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128());
  return _mm_xor_si128(_mm_add_epi8(x, x), x);
}

static inline void sse_NOT(const bool *const a, const bool *const dest, const int N) {
  assert(N % 16 == 0);
	
  const __m128i* ptr = (__m128i*)a;
  const __m128i* ptr_end = (__m128i*)(a + N);
  __m128i* dst_ptr = (__m128i*)dest;

  __m128i vector_of_ones = _mm_setone_epi8();

  while(ptr < ptr_end) {
    __m128i result = _mm_xor_si128(XMMLOAD_ALIGNED(ptr), vector_of_ones);
    _mm_storeu_si128(dst_ptr, result);
    ptr++;
    dst_ptr++;
  }
}

static inline void sse_AND(const bool *const a, const bool *const b, const bool *const dest, const int N) {
  assert(N % 16 == 0);

  const __m128i* ptr = (__m128i*)a;
  const __m128i* ptr_end = (__m128i*)(a + N);
  const __m128i* ptr2 = (__m128i*)b;
  __m128i* dst_ptr = (__m128i*)dest;

  while (ptr < ptr_end) {
    __m128i result = _mm_and_si128(XMMLOAD_ALIGNED(ptr), XMMLOAD_ALIGNED(ptr2));
    _mm_storeu_si128(dst_ptr, result);
    ptr++;
    ptr2++;
    dst_ptr++;
  }
}

static inline void sse_OR(const bool *const a, const bool *const b, const bool *const dest, const int N) {
  assert(N % 16 == 0);

  const __m128i* ptr = (__m128i*)a;
  const __m128i* ptr_end = (__m128i*)(a + N);
  const __m128i* ptr2 = (__m128i*)b;
  __m128i* dst_ptr = (__m128i*)dest;

  while (ptr < ptr_end) {
    __m128i result = _mm_or_si128(XMMLOAD_ALIGNED(ptr), XMMLOAD_ALIGNED(ptr2));
    _mm_storeu_si128(dst_ptr, result);
    ptr++;
    ptr2++;
    dst_ptr++;
  }
}

static inline void sse_CLEAR(bool *const buffer, const int N) {
  __m128i zero = _mm_setzero_si128();

  for (__m128i* start = reinterpret_cast<__m128i*>(buffer),
	 *end = reinterpret_cast<__m128i*>(&buffer[N]);
       start < end; start++) {
    _mm_store_si128(start, zero);
  }
}

static inline void clearWithMemset(bool *const buffer, const int N) {
  memset(buffer, 0, N);
}

static inline void sse_SETVAL(int* buffer, const bool *const mask, const int N, const int val) {
  assert(N % 16 == 0);

  const __m128i value = _mm_set1_epi32(val);
  const __m128i* buffer_ptr = (__m128i*)buffer;
  const __m128i* buffer_end_ptr = (__m128i*)(buffer + N);
  int* output_buffer_ptr = buffer;
  const bool* mask_ptr = mask;

  while(buffer_ptr < buffer_end_ptr) {
    __m128i mask_value = XMMLOAD(mask_ptr);
    __m128i unpacked_mask = _mm_unpacklo_epi16(_mm_unpacklo_epi8(mask_value, _mm_setzero_si128()), _mm_setzero_si128());
    __m128i buffer_mask = _mm_or_si128(_mm_slli_epi32(unpacked_mask, 31), _mm_srli_epi32(unpacked_mask, 31));

    __m128i result = _mm_add_epi32(value, XMMLOAD_ALIGNED(buffer_ptr));
    _mm_maskstore_epi32(output_buffer_ptr, buffer_mask, result);

    buffer_ptr++;
    output_buffer_ptr += 4;
    mask_ptr += 4;
  }
}

static inline bool sse_IS_ZERO(const bool *const buffer, const int N) {
  assert(N % 16 == 0);

  const __m128i zeros = _mm_setzero_si128();
  const __m128i* buffer_ptr = (__m128i*)buffer;
  const __m128i* buffer_end_ptr = (__m128i*)(buffer + N);

  while(buffer_ptr < buffer_end_ptr) {
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(XMMLOAD_ALIGNED(buffer_ptr++), zeros)) != 0xffff) 
      return false;
  }
  return true;
}

static void run_bfs(bool *const visited, int* distance, const int N, const int roundedN, int source, 
		    bool *const X_, bool *const Y, bool *const Z, bool *const notvisited) {

  visited[source] = true;
  distance[source] = 0;

  X_[source] = true;

  for (int level = 1; level < N; level++) {
    clearWithMemset(Y, roundedN);

    for (int j = 0; j < N; j++) {
      if (X_[j] == true) {
	sse_NOT(visited, notvisited, roundedN);
	sse_AND(adj[j], notvisited, Z, roundedN);
	sse_OR(Y, Z, Y, roundedN);
      }
    }

    sse_OR(visited, Y, visited, roundedN);
    memcpy(X_, Y, roundedN);
    sse_SETVAL(distance, Y, roundedN, level);

    if (sse_IS_ZERO(X_, roundedN)) {
      break;
    }
  }
}

static inline int roundUpToSixteen(const int n) {
  return (n + 16 - 1) & ~(16 - 1);
}

static int bfsavx2_load_graph(std::vector<int> vertices,  std::vector<std::pair<int,int>> graph )
{
  int undirect = 0;

  NN = vertices.size();
  QQ = graph.size();

  roundedNN = roundUpToSixteen(NN);
  
  adj = new bool*[roundedNN];
  if (adj == NULL) {
    cout << "Cannot allocated memory for graph representation matrix" << endl;
    return 1;
  }

  for (int i = 0; i < roundedNN; i++) {
    adj[i] = (bool *)_aligned_malloc(roundedNN * sizeof(bool), 16);

    if (adj[i] == NULL) {
      cout << "Cannot allocated memory for graph representation matrix" << endl;
      return 1;
    }
    
    clearWithMemset(adj[i], roundedNN);
  }
	
  int a, b;
  for (int i = 0; i < QQ; i++) {
    a = graph[i].first;
    b = graph[i].second;
    if (undirect)
      adj[a][b] = adj[b][a] = true;
    else
      adj[a][b] = true;
  }

  visited = static_cast<bool *>(_aligned_malloc(roundedNN, 16));
  negvisited = static_cast<bool *>(_aligned_malloc(roundedNN, 16));
  distance_ = static_cast<int *>(_aligned_malloc(roundedNN * sizeof(int), 16));
  Y = static_cast<bool *>(_aligned_malloc(roundedNN, 16));
  X_ = static_cast<bool *>(_aligned_malloc(roundedNN, 16));
  Z = static_cast<bool *>(_aligned_malloc(roundedNN, 16));
}

static int bfsavx2_done() {
  for(int i = 0; i < roundedNN; i++) {
    _aligned_free(adj[i]);
  }
	
  //	delete[] time_results;

  delete[] adj;

  _aligned_free(visited);
  _aligned_free(negvisited);
  _aligned_free(distance_);

  _aligned_free(Z);
  _aligned_free(Y);
  _aligned_free(X_);
}

static int bfsavx2_reset() 
{
  for (int i = 0; i < roundedNN; i++) {
    clearWithMemset(adj[i], roundedNN);
  }
  clearWithMemset(visited, roundedNN);
  memset(distance_, 0, sizeof(int) * roundedNN);
  clearWithMemset(X_, roundedNN);
  clearWithMemset(Z, roundedNN);
  clearWithMemset(Y, roundedNN);
}

static int bfsavx2_retry() 
{
  clearWithMemset(visited, roundedNN);
  memset(distance_, 0, sizeof(int) * roundedNN);
  clearWithMemset(X_, roundedNN);
  clearWithMemset(Z, roundedNN);
  clearWithMemset(Y, roundedNN);
}

static int bfsavx2_distance_alloc_count() { return roundedNN; }
static int bfsavx2_distance_alloc_size() { return sizeof(int) * roundedNN; }
static int bfsavx2_distance_count() { return NN; }

static int bfsavx2_compute_distance(int START_VERTEX, int *d) 
{
  bfsavx2_retry() ;
  run_bfs(visited, distance_, NN, roundedNN, START_VERTEX, X_, Y, Z, negvisited);
  distance_[START_VERTEX] = -1;
  memcpy(d, distance_, bfsavx2_distance_alloc_size());
  return 0;
}



int bfs_avx2::load_graph(std::vector<int>& vertices,  std::vector<std::pair<int,int>>& graph ) {
  return bfsavx2_load_graph(vertices, graph);
}
int bfs_avx2::done() { return bfsavx2_done(); }
int bfs_avx2::reset() { return bfsavx2_reset(); }
int bfs_avx2::retry() { return bfsavx2_retry(); }
int bfs_avx2::distance_alloc_count() { return bfsavx2_distance_alloc_count(); }
int bfs_avx2::distance_alloc_size()  { return bfsavx2_distance_alloc_size(); }
int bfs_avx2::distance_count() { return bfsavx2_distance_count(); }
int bfs_avx2::traverse(int START_VERTEX, int *d) {
  return bfsavx2_compute_distance(START_VERTEX, d); 
}


