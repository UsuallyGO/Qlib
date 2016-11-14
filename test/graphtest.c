
#include <stdio.h>
#include "qlib/qgraph.h"
#include "qlib/qmemory.h"

QEdge edges[] = {
	{0, 2, 1}, {2, 0, 1},
	{0, 5, 1}, {5, 0, 1},
	{0, 7, 1}, {7, 0, 1},
	{2, 6, 1}, {6, 2, 1},
	{6, 4, 1}, {4, 6, 1},
	{4, 3, 1}, {3, 4, 1},
	{4, 7, 1}, {7, 4, 1},
	{4, 5, 1}, {5, 4, 1},
	{7, 1, 1}, {1, 7, 1},
	{3, 5, 1}, {5, 3, 1},
};

QEdge edges2[] = {
	{0, 1, 4}, {1, 0, 4},
	{0, 5, 7}, {5, 0, 7},
	{0, 6, 6}, {0, 6, 6},
	{0, 2, 3}, {2, 0, 3},
	{0, 7, 3}, {7, 0, 3},
	{1, 7, 1}, {7, 1, 1},
	{7, 6, 1}, {7, 6, 1},
	{7, 4, 5}, {7, 4, 5},
	{3, 5, 1}, {5, 3, 1},
	{3, 4, 2}, {4, 3, 2},
	{6, 4, 5}, {4, 6, 5},
	{5, 4, 4}, {4, 5, 4},
};

QEdge edges3[] = {
	{0, 2, 6},
	{0, 4, 7},
	{2, 3, 5},
	{3, 2, -2},
	{2, 1, -4},
	{2, 4, 8},
	{1, 0, 2},
	{1, 3, 7},
	{4, 3, -3},
	{4, 1, 9},
};

QEdge edges4[] = {
	{0, 3, 10},
	{0, 1, 5},
	{3, 2, 1},
	{3, 1, 2},
	{1, 3, 3},
	{4, 0, 7},
	{1, 2, 9},
	{1, 4, 2},
	{2, 4, 4},
	{4, 2, 6},
};

int main()
{
	int i, cnt;
	QGraph *g;
	QEdge *edge;

	g = QGraph_create();
	for(i = 0; i < sizeof(edges)/sizeof(QEdge); i++)
		QGraph_insert(g, edges[i].u, edges[i].v, edges[i].w);
	QGraph_show(g);

	cnt = QGraph_searchDFS(g, 0, &edge);
	printf("DFS search result:\n");
	for(i = 0; i < cnt; i++)
		printf("%-2d-->%-2d\n", edge[i].u, edge[i].v);
	QMem_Free(edge);

    cnt = QGraph_searchBFS(g, 0, &edge);
    printf("BFS search result:\n");
    for(i = 0; i < cnt; i++)
        printf("%-2d-->%-2d\n", edge[i].u, edge[i].v);
	QMem_Free(edge);
	QGraph_destroy(g);

	g = QGraph_create();
	for(i = 0; i < sizeof(edges2)/sizeof(QEdge); i++)
		QGraph_insert(g, edges2[i].u, edges2[i].v, edges2[i].w);
	QGraph_show(g);

	cnt = QGraph_prim(g, 0, &edge);
	printf("Prime search result:\n");
    for(i = 0; i < cnt; i++)
        printf("%-2d-->%-2d\n", edge[i].u, edge[i].v);
	QMem_Free(edge);

	cnt = QGraph_kruskal(g, 0, &edge);
	printf("Kruskal search result:\n");
    for(i = 0; i < cnt; i++)
        printf("%-2d-->%-2d\n", edge[i].u, edge[i].v);
	QMem_Free(edge);
	QGraph_destroy(g);

	g = QGraph_create();
	for(i = 0; i < sizeof(edges3)/sizeof(QEdge); i++)
		QGraph_insert(g, edges3[i].u, edges3[i].v, edges3[i].w);
	QGraph_show(g);

	cnt = QGraph_bellmanford(g, 0, &edge);
	printf("Bell-man-Ford result:\n");
	for(i = 0; i < cnt; i++)
		printf("%-2d-->%-2d:%-2d  Successor:%d\n", 0, edge[i].v, edge[i].w, edge[i].u);
	QMem_Free(edge);
	QGraph_destroy(g);

	g = QGraph_create();
	for(i = 0; i < sizeof(edges4)/sizeof(QEdge); i++)
		QGraph_insert(g, edges4[i].u, edges4[i].v, edges4[i].w);
	QGraph_show(g);

	cnt = QGraph_bellmanford(g, 0, &edge);
	printf("Dijkstra result:\n");
	for(i = 0; i < cnt; i++)
		printf("%-2d-->%-2d:%-2d  Successor:%d\n", 0, edge[i].v, edge[i].w, edge[i].u);
	QMem_Free(edge);
	QGraph_destroy(g);	

	QMemDumpInfo(0);
	return 0;
}
