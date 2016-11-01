
#ifndef _QLIB_LIST_H_
#define _QLIB_LIST_H_

#include "qgeneral.h"

typedef struct List_T QList;
typedef struct Node_T QNode;
typedef void NodeData;

struct Node_T{
    QNode     *m_prev;
    QNode     *m_next;
    NodeData  *m_data;
};

struct List_T{
    QNode *m_head;
    QNode *m_tail;
    unsigned int m_count;

    DataCompFunc dataCompare;
    DataDestroyFunc dataDestroy;
};

typedef struct{
    QList *m_list;
    QNode *m_node;
    unsigned int m_index;
}QListIterator;

void  QList_init(QList *list, DataCompFunc compare, DataDestroyFunc destroy);
void  QList_addHead(QList *list, void *data);
void  QList_addTail(QList *list, void *data);
void* QList_removeHead(QList *list);
void* QList_removeTail(QList *list);
int   QList_insertBefore(QList *list, void *before, void *data);
int   QList_insertAfter(QList *list, void *after, void *data);
int   QList_remove(QList *list, void *data);
void  QList_clearAll(QList *list);
int   QList_getCount(QList *list);
int   QList_find(QList *list, void *data);
void  QListIterReset(QListIterator *iter, QList *list);
void* QListIterNext(QListIterator *iter);

#endif //!_QLIB_LIST_H_