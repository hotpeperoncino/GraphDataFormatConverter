
#include "bfs_cpu.h"

typedef struct vertex_t
{
  int id;
  int count;
  int *adjacent;
} vertex_t;

typedef struct element_t
{
  vertex_t *value;
  struct element_t *next;
} element_t;

typedef struct queue_t
{
  element_t *head;
  element_t *tail;
} queue_t;


static queue_t *queueInit(void);
static void queuePush(queue_t *qq, vertex_t *value);
static vertex_t *queuePop(queue_t *qq);
static void queueDelete(queue_t *qq);
static int queueIsEmpty(queue_t *qq);

#include <memory.h>

typedef int visited_t;

typedef struct graphbfs
{
  vertex_t *vertices;
  visited_t *visited;
  int size;
} graphbfs;

static graphbfs *graphBFSInit(void);
static void graphBFSAddEdge(graphbfs *gbfs, int s, int e);
static void graphBFSDelete(graphbfs *gbfs);
static int graphBFSGetSize(graphbfs *gbfs);



static queue_t *queueInit(void)
{
  queue_t *temp = (queue_t*)calloc(1, sizeof(queue_t));
  return temp;
}

static void queuePush(queue_t *qq,vertex_t *value)
{
  element_t *temp = (element_t*)calloc(1, sizeof(element_t));
  temp->value = value;
  if (qq->head == NULL)
    qq->head = temp;
  else
    qq->tail->next = temp;
  qq->tail = temp;
}

static vertex_t *queuePop(queue_t *qq)
{
  // assert queue not empty
  vertex_t *result = qq->head->value;
  element_t *temp = qq->head;
  qq->head = qq->head->next;
  free(temp);
  return result;
}

static void queueDelete(queue_t *qq)
{
  element_t *it = qq->head;
  element_t  *temp = NULL;
  while (it != NULL)
    {
      temp = it->next;
      free(it);
      it = temp;
    }
  free(qq);
}

static int queueIsEmpty(queue_t *qq)
{
  return !qq->head;
}

static graphbfs *graphBFSInit(int num)
{
  graphbfs *temp = (graphbfs*)calloc(num, sizeof(graphbfs));
  int i;
  temp->size = num;
  temp->vertices = (vertex_t*)malloc(num * sizeof(vertex_t));
  temp->visited = (visited_t*)malloc(num * sizeof(visited_t));
  for (i = 0; i < num; i++)
    {
      temp->vertices[i].count = 0;
      temp->vertices[i].id = i;
      temp->vertices[i].adjacent = NULL;
    }
  memset(temp->visited, 0, num * sizeof(visited_t));
  return temp;
}


static void graphBFSAddEdge(graphbfs *gbfs, int id, int e)
{
  int i = 0;
  int found = 0;
  for (i = 0; i < gbfs->vertices[id].count; i++)
    {
      if (gbfs->vertices[id].adjacent[i] == e)
	found = 1;
    }
  if (found == 0)
    {
      gbfs->vertices[id].adjacent = (int*)realloc(gbfs->vertices[id].adjacent, (gbfs->vertices[id].count + 1) * sizeof(int));
      gbfs->vertices[id].adjacent[gbfs->vertices[id].count] = e;
      gbfs->vertices[id].count++;
    }
}

static int graphBFSGetSize(graphbfs *gbfs)
{
  return gbfs->size;
}

static void graphBFSDelete(graphbfs *gbfs)
{
  int i = 0;
  for (i = 0; i < gbfs->size; i++)
    {
      if (gbfs->vertices[i].count > 0)
	free(gbfs->vertices[i].adjacent);
    }
  free(gbfs->vertices);
  free(gbfs);
}





static void dobfs(graphbfs *gbfs, int id)
{
  int i = 0;
  vertex_t *vertex = NULL;
  queue_t *queue = queueInit();
  //  printf("dobfs %d\n", id);
  queuePush(queue, &gbfs->vertices[id]);
  while (!queueIsEmpty(queue))
    {
      vertex = queuePop(queue);
      if (gbfs->visited[vertex->id] == 1)
	continue;
      gbfs->visited[vertex->id] = 1;
      //      printf("%d ", vertex->id);
      for (i = 0; i < vertex->count; i++)
	{
	  queuePush(queue, &gbfs->vertices[vertex->adjacent[i]]);
	}
    }
  queueDelete(queue);
}


int bfs_cpu::load_graph(std::vector<int>& vdata,  std::vector<std::pair<int,int>>& graph ) {
  vertex = graphBFSInit(vdata.size());
  n = graphBFSGetSize(vertex);
  for (int i = 0; i < graph.size(); i++) {
    graphBFSAddEdge(vertex, graph[i].first, graph[i].second);
  }
}
int bfs_cpu::done() {
  graphBFSDelete(vertex);
}
int bfs_cpu::retry() {  
  memset(vertex->visited, 0, n * sizeof(visited_t));
}
int bfs_cpu::distance_alloc_count() { return n; }
int bfs_cpu::distance_alloc_size()  { return (n) * sizeof(int); }
int bfs_cpu::distance_count() { return n; }
int bfs_cpu::traverse(int START_VERTEX, int *d) {
  //    printf("START_VERTEX=%d\n", START_VERTEX);
  retry();
  dobfs(vertex, START_VERTEX);
  memcpy(d, vertex->visited, n* sizeof(visited_t));
  return 0;
}



