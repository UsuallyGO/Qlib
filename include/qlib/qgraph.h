
#ifndef _QSTL_GRAPH_H_
#define _QSTL_GRAPH_H_

#include <stddef.h>

#define QGRAPH_DEFAULT_SIZE 10
#define QGRAPH_DEFAULT_EDGES 10

struct _Edge_t{
	int u;
	int v;
	int w;
};
typedef struct _Edge_t QEdge;

struct _Graph_t{
	size_t vertexs;
	size_t edges;
	size_t gsize;
	int**  matrix;
};
typedef struct _Graph_t QGraph;

QGraph* QGraph_create();
int QGraph_insert(QGraph* g, int u, int v, int w);
int QGraph_searchDFS(QGraph *g, int v, QEdge** tr);
int QGraph_searchBFS(QGraph *g, int v, QEdge** tr);
int QGraph_prime(QGraph* g, int v, QEdge** tr);
int QGraph_kruskal(QGraph* g, int v, QEdge** tr);
void QGraph_destroy(QGraph* g);
void QGraph_show(QGraph *g);

#endif //!_QSTL_GRAPH_H_