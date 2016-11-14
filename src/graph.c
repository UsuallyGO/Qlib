
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "qlib/qgeneral.h"
#include "qlib/qmemory.h"
#include "qlib/qgraph.h"
#include "qlib/qstack.h"
#include "qlib/qlist.h"
#include "qlib/qsort.h"

QGraph* QGraph_create(){
	int i;
	QGraph* g;

	g = (QGraph*)QMem_Malloc(sizeof(QGraph));
	assert(g);
	g->vertexs = 0;
	g->edges = 0;
	g->gsize = QGRAPH_DEFAULT_SIZE;
	g->matrix = (int**)QMem_Malloc(sizeof(int*)*g->gsize);
	assert(g->matrix);
	for(i = 0; i < g->gsize; i++)
	{
		g->matrix[i] = (int*)QMem_Malloc(sizeof(int)*g->gsize);
		memset(g->matrix[i], 0, g->gsize*sizeof(int));
		assert(g->matrix[i]);
	}
	return g;
}

int QGraph_insert(QGraph *g, int u, int v, int w){
	if(g == NULL)
		return QLIB_ERR_INVAL;
	if(u >= g->gsize || v >= g->gsize || u < 0 || v < 0)
		return QLIB_ERR_INVAL;
	if(g->matrix[u][v] == w)
		return QLIB_ERR_DONE;
	if(g->matrix[u][v] != 0)
		return QLIB_ERR_DUPLICATE;

	g->matrix[u][v] = w;
	g->edges++;
	u = u > v ? u : v;//u is the bigger one
	if(g->vertexs <= u)
		g->vertexs = u + 1;//vertexs is the nodes count, not the biggest vertex number

	return QLIB_SUCCEEDED;
}

int QGraph_searchDFS(QGraph *g, int v, QEdge** tr)
{
	QStack st;
	QEdge *edge;
	int *visit;
	int cnt;

	if(g == NULL || tr == NULL || v >= g->vertexs)
		return 0;

	visit = (int*)QMem_Malloc(g->vertexs*sizeof(int));
	assert(visit);
	memset(visit, 0, g->vertexs*sizeof(int));
	edge = (QEdge*)QMem_Malloc((g->vertexs-1)*sizeof(QEdge));
	assert(edge);
	cnt = 0;
	QStack_init(&st);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
	QStack_push(&st, (void*)v);
#pragma GCC diagnostic pop
	while(!QStack_empty(&st))
	{
		int i, n;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
		n = (int)QStack_top(&st);
#pragma GCC diagnostic pop
		visit[n] = 1;
		for(i = 0; i < g->vertexs; i++)
			if(g->matrix[n][i] && !visit[i])
			{
				edge[cnt].u = n;
				edge[cnt].v = i;
				cnt++;
				visit[i] = 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
				QStack_push(&st, (void*)i);
#pragma GCC diagnostic pop
				break;
			}
		if(i >= g->vertexs)
			QStack_pop(&st);
	}

	QMem_Free(visit);
	*tr = edge;
	return cnt;
}

int QGraph_searchBFS(QGraph *g, int v, QEdge** tr)
{
	QList list;
	QEdge *edge;
	int cnt;
	int *visit;

	if(g == NULL || v >= g->vertexs || tr == NULL)
		return QLIB_ERR_INVAL;

	QList_init(&list, NULL, NULL);
	visit = (int*)QMem_Malloc(sizeof(int)*g->gsize);
	assert(visit);
	memset(visit, 0, sizeof(int)*g->gsize);
	edge = (QEdge*)QMem_Malloc(sizeof(QEdge)*(g->vertexs-1));
	assert(edge);
	cnt = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"	
	QList_addTail(&list, (void*)v);
#pragma GCC diagnostic pop

	visit[v] = 1;
	while(QList_getCount(&list))
	{
		int i, n;
		QListIterator iter;
		QListIterReset(&iter, &list);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
		n = (int)QListIterNext(&iter);
#pragma GCC diagnostic pop
		QList_removeHead(&list);
		for(i = 0; i < g->vertexs; i++)
			if(g->matrix[n][i] && !visit[i])
			{
				edge[cnt].u = n;
				edge[cnt].v = i;
				cnt++;
				visit[i] = 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
				QList_addTail(&list, (void*)i);
#pragma GCC diagnostic pop
			}
	}

	QMem_Free(visit);
	*tr = edge;
	return cnt;
}

int QGraph_prim(QGraph* g, int v, QEdge** tr)
{
	QEdge *edge;
	int cnt;
	int *visit;
	int *min, *mediate;
	int v_tmp, i;

	if(g == NULL || v >= g->vertexs || tr == NULL)
		return QLIB_ERR_INVAL;

	visit = (int*)QMem_Malloc(g->vertexs*sizeof(int));
	assert(visit);
	memset(visit, 0, g->vertexs*sizeof(int));
	min = (int*)QMem_Malloc(g->vertexs*sizeof(int));
	assert(min);
	mediate =(int*)QMem_Malloc(g->vertexs*sizeof(int));
	assert(mediate);
	visit[v] = 1;
	for(i = 0; i < g->vertexs; i++)
	{
		if(g->matrix[v][i])
		{
			min[i] = g->matrix[v][i];
			mediate[i] = v;
		}
		else
		{
			min[i] = INT_MAX;
			mediate[i] = -1;
		}
	}

	edge = (QEdge*)QMem_Malloc((g->vertexs-1)*sizeof(QEdge));
	assert(edge);
	memset(edge, 0, (g->vertexs-1)*sizeof(QEdge));
	cnt = 0;
	while(cnt < g->vertexs-1)
	{
		int min_tmp = INT_MAX;
		for(i = 0; i < g->vertexs; i++)
			if(!visit[i] && min[i] < min_tmp)
				v_tmp = i, min_tmp = min[v_tmp];

		v = mediate[v_tmp];
		edge[cnt].u = v, edge[cnt].v = v_tmp;
		edge[cnt].w = min[v_tmp];
		visit[v_tmp] = 1;
		cnt++;
		for(i = 0; i < g->vertexs; i++)
			if(!visit[i] && g->matrix[v_tmp][i] && min[i] > g->matrix[v_tmp][i])
			{
				min[i] = g->matrix[v_tmp][i];
				mediate[i] = v_tmp;
			}
	}

	QMem_Free(min);
	QMem_Free(mediate);
	QMem_Free(visit);
	*tr = edge;
	return cnt;
}

static int _graph_kruskal_cmp(const void *data1, const void *data2)
{
	QEdge *edge1, *edge2;
	edge1 = (QEdge*)data1, edge2 = (QEdge*)data2;

	return edge1->w - edge2->w;
}

int QGraph_kruskal(QGraph* g, int v, QEdge** tr)
{
	QEdge *edge_tmp, *edge;
	int *con;
	int edge_cnt, cnt;
	int i, j;
	QSort sort;

	if(g == NULL || v >= g->vertexs || tr == NULL)
		return QLIB_ERR_INVAL;

	edge = (QEdge*)QMem_Malloc(sizeof(QEdge)*(g->vertexs-1));
	assert(edge);
	edge_tmp = (QEdge*)QMem_Malloc(g->edges*sizeof(QEdge));
	assert(edge_tmp);
	con = (int*)QMem_Malloc(g->vertexs*sizeof(int));
	assert(con);

	edge_cnt = 0, cnt = 0;
	for(i = 0; i < g->vertexs; i++)
		con[i] = -i-1;//avoid for 0
	for(i = 0; i < g->vertexs; i++)
		for(j = 0;j < g->vertexs; j++)
		{
			if(g->matrix[i][j])
			{
				edge_tmp[edge_cnt].u = i;
				edge_tmp[edge_cnt].v = j;
				edge_tmp[edge_cnt].w = g->matrix[i][j];
				edge_cnt++;
			}
		}
	if(edge_cnt != g->edges)
		goto Err_edges;
		
	QSort_config(&sort, QSORT_ASCEND, QSORT_SORT_QUICK);
    QSort_set(&sort, edge_tmp, 0, edge_cnt, sizeof(QEdge), _graph_kruskal_cmp);
	QSort_sort(&sort);

	for(i = 0; i < edge_cnt; i++)
	{
		if(cnt >= g->vertexs-1)
			break;

		if(con[edge_tmp[i].u] != con[edge_tmp[i].v])
		{
			edge[cnt] = edge_tmp[i];
			if(edge_tmp[i].v > 0)
				con[edge_tmp[i].u] = con[edge_tmp[i].v];
			else if(edge_tmp[i].u > 0)
				con[edge_tmp[i].v] = con[edge_tmp[i].u];
			else
			{
				con[edge_tmp[i].u] = -con[edge_tmp[i].u];
				con[edge_tmp[i].v] = con[edge_tmp[i].u];
			}
			cnt++;
		}
	}

	if(cnt != g->vertexs-1)
		goto Err_connected;

	QMem_Free(edge_tmp);
	QMem_Free(con);
	*tr =edge;
	return cnt;

Err_connected:
	QMem_Free(edge);
Err_edges:
	QMem_Free(edge_tmp);
	QMem_Free(con);
	return QLIB_ERR_INVAL;
}

int QGraph_bellmanford(QGraph *g, int v, QEdge** tr)
{
	QEdge *edge;
	int index,i,j;

	if(g == NULL || v >= g->vertexs || tr == NULL)
		return QLIB_ERR_INVAL;

	edge = (QEdge*)QMem_Malloc((g->vertexs)*sizeof(QEdge));
	assert(edge);

	for(index = 0; index< g->vertexs; index++)
	{
		edge[index].u = v;
		edge[index].v = index;
		edge[index].w = INT_MAX;
	}
	edge[v].w = 0;

	for(index = 0; index < g->vertexs - 1; index++)
	{
		for(i = 0; i < g->vertexs; i++)
			for(j = 0; j <g->vertexs; j++)
			{
				if(g->matrix[i][j] && edge[i].w != INT_MAX
					&& edge[j].w > (edge[i].w + g->matrix[i][j]))
				{
					edge[j].w = edge[i].w + g->matrix[i][j];
					edge[j].u = i;
				}
			}
	}

	for(i = 0; i < g->vertexs; i++)
		for(j = 0; j < g->vertexs; j++)
		{
			if(g->matrix[i][j] && edge[i].w != INT_MAX
				&& edge[j].w > (edge[i].w + g->matrix[i][j]))
			{
				*tr = NULL;
				return QLIB_ERR_DONE;
			}
		}

	*tr = edge;
	return g->vertexs;
}

int QGraph_dijkstra(QGraph *g, int v, QEdge** tr)
{
	QEdge *edge;
	int *visit;
	int cnt, i, j, min_v;

	visit = (int*)QMem_Malloc(sizeof(int)*g->vertexs);
	assert(visit);
	edge = (QEdge*)QMem_Malloc(sizeof(QEdge)*g->vertexs);
	assert(visit);
	memset(visit, 0, sizeof(int)*g->vertexs);
	visit[v] = 1;

	for(i = 0; i < g->vertexs; i++)
	{
		edge[i].u = v;
		edge[i].v = i;
		if(g->matrix[v][i])
			edge[i].w = g->matrix[v][i];
		else
			edge[i].w = INT_MAX;
	}
	edge[v].w = 0;

	if(v != 0)//min_v can be arbitray value exclude for v
		min_v = 0;
	else
		min_v = g->vertexs - 1;

	cnt = 1;
	while(cnt < g->vertexs)//v is already visited
	{
		for(i = 0; i < g->vertexs; i++)
		{
			if(!visit[i] && edge[i].w < edge[min_v].w)
				min_v = i;
		}

		visit[min_v] = 1;
		cnt++;

		for(i = 0; i < g->vertexs; i++)
		{
			if(g->matrix[min_v][i] && edge[min_v].w != INT_MAX
				&& edge[i].w > (edge[min_v].w + g->matrix[min_v][i]))
			{
				edge[i].u = min_v;
				edge[i].v = i;
				edge[i].w = edge[min_v].w + g->matrix[min_v][i];
			}
		}
	}

	*tr = edge;
	return cnt;
}

void QGraph_destroy(QGraph* g)
{
	if(g != NULL)
	{
		int i;
		for(i = 0; i < g->gsize; i++)
			QMem_Free(g->matrix[i]);
		QMem_Free(g->matrix);
		QMem_Free(g);
	}
}

void QGraph_show(QGraph *g)
{
	if(g != NULL)
	{
		int i, j;
		printf("  ");
		for(i = 0; i < g->vertexs; i++)
			printf(" %-2d", i);
		printf("\n");
		for(i = 0; i < g->vertexs; i++)
		{
			printf("%-2d", i);
			for(j = 0; j < g->vertexs; j++)
				printf(" %-2d", g->matrix[i][j]);
			printf("\n");
		}
	}
}
