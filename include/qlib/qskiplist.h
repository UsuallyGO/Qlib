
#ifndef _QLIB_SKIPLIST_H_
#define _QLIB_SKIPLIST_H_

#include "qlib/qgeneral.h"

#define QSKIPLIST_MAX_LEVELS 30

typedef struct SkipList_T QSkiplist;
typedef struct SkipList_Node_T QSkiplistNode;

#pragma pack(1)
struct SkipList_Node_T
{
    void* m_data;
    QSkiplistNode *m_next[1];//more than only one next
};
#pragma pack()

struct SkipList_T
{
    int m_length; //the total number of elements in the skiplist
    int m_levels; //the current max level in the skiplist

    QSkiplistNode *m_head;
    QSkiplistNode *m_tail;

    DataCompFunc cmpf;
    DataDestroyFunc desf;

    /*Use struct not pointers here to simplify the code in insert and delete*/
    QSkiplistNode m_link[QSKIPLIST_MAX_LEVELS];
};

QSkiplist* QSkiplist_create(DataCompFunc cmp, DataDestroyFunc des);

int QSkiplist_insert(QSkiplist *slist, void *data);

void QSkiplistPrint(QSkiplist *slist, void (*dataPrint)(void*));

void QSkiplist_destroy(QSkiplist *slist);

#endif //!_QLIB_SKIPLIST_H_
