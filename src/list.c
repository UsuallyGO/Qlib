
#include <assert.h> //assert
#include <stdio.h> //NULL
#include <stdlib.h> //free
#include "qlib/qmemory.h"
#include "qlib/qgeneral.h"
#include "qlib/qlist.h"

void QList_init(QList *list, DataCompFunc compare, DataDestroyFunc destroy)
{
    assert(list);

    list->m_head  = NULL;
    list->m_tail  = NULL;
    list->m_count = 0;
    list->dataCompare = compare; //dataCompare and dataDestroy can be NULL
    list->dataDestroy = destroy;
}

static QNode* _NodeCreate(void* data)
{
    QNode *node;

    node = (QNode*)QMem_Malloc(sizeof(QNode));
    assert(node);
    node->m_data = data;
    node->m_next = NULL;
    node->m_prev = NULL;
    return node;
}

void QList_addHead(QList *list, void *data)
{
    QNode *node;
    assert(list && data);

    node = _NodeCreate(data);
    node->m_next = list->m_head;

    if(list->m_head == NULL)
        list->m_tail = node;
    else
        list->m_head->m_prev = node;
    list->m_head = node;
    list->m_count++;
}

void QList_addTail(QList *list, void *data)
{
    QNode *node ;
    assert(list && data);

	node = _NodeCreate(data);
    node->m_prev = list->m_tail;

    if(list->m_tail == NULL)//empty list
        list->m_head = node;
    else
        list->m_tail->m_next = node;

    list->m_tail = node;
    list->m_count++;
}

void* QList_removeHead(QList *list)
{
    QNode *node;
    void  *data;
    assert(list);

    if(list->m_count == 0)
        return NULL;

    node = list->m_head;
    list->m_head = node->m_next;
    if(list->m_count == 1) // node->m_next is NULL, now list is empty
        list->m_tail = NULL;
    else
        node->m_next->m_prev = NULL;
    list->m_count--;
    data = node->m_data;
    QMem_Free(node);
    return data;
}

void* QList_removeTail(QList *list)
{
    QNode *node;
    void  *data;
    assert(list);

    if(list->m_count == 0)
        return NULL;

    node = list->m_tail;
    list->m_tail = node->m_prev;
    if(list->m_count == 1)
        list->m_head = NULL;
    else
        list->m_tail->m_next = NULL;
    list->m_count--;
    data = node->m_data;
    QMem_Free(node);
    return data;
}

static QNode* _ListFindNode(QList *list, void *data)
{
    QNode *node;

    for(node = list->m_head; node != NULL; node = node->m_next)
        if(list->dataCompare != NULL)
            if(!list->dataCompare(node->m_data, data))
                break;
    return node;
}

int QList_insertBefore(QList *list, void *before, void *data)
{
    QNode *beforeNode, *node;
    assert(list);
    assert(before && data);

    beforeNode = _ListFindNode(list, before);
    if(beforeNode == NULL)
        return QLIB_ERR_NOTFOUND; 
    node = _NodeCreate(data);

    node->m_next = beforeNode;
    node->m_prev = beforeNode->m_prev;
    if(beforeNode->m_prev != NULL) //head doesn't need to adjust
        beforeNode->m_prev->m_next = node;
    else //beforeNode is list head
        list->m_head = node;
    beforeNode->m_prev = node;
    list->m_count++;
    return QLIB_SUCCEEDED;
}

int QList_insertAfter(QList *list, void *after, void *data)
{
    QNode *afterNode, *node;
    assert(list);
    assert(after && data);

    afterNode = _ListFindNode(list, after);
    if(afterNode == NULL)
        return QLIB_ERR_NOTFOUND;
    node = _NodeCreate(data);

    node->m_prev = afterNode;
    node->m_next = afterNode->m_next;
    if(afterNode->m_next != NULL) //tail doesn't need to adjust
        afterNode->m_next->m_prev = node;
    else //afterNode is list tail
        list->m_tail = node;
    afterNode->m_next = node;
    list->m_count++;
    return QLIB_SUCCEEDED;
}

int QList_remove(QList *list, void *data)
{
    QNode *node, *prev, *next;
    assert(list && data);

    node = _ListFindNode(list, data);
    if(node == NULL)
        return QLIB_ERR_NOTFOUND;
    prev = node->m_prev;
    next = node->m_next;
    if(prev == NULL)//node is head, delete head now
        list->m_head = node->m_next;
    else
        prev->m_next = node->m_next;
    if(next == NULL)//node is tail, delete tail now
        list->m_tail = node->m_prev;
    else
        next->m_prev = node->m_prev;
    list->m_count--;

    QMem_Free(node);
    return QLIB_SUCCEEDED;
}

void QList_clearAll(QList *list)
{
    QNode    *node;
    assert(list);

    do{
        node = list->m_head;
        if(node == NULL)
            break;
        list->m_head = node->m_next;
        if(list->dataDestroy != NULL)
            list->dataDestroy(node->m_data);
        QMem_Free(node);
    }while(1);

    list->m_head = NULL;
    list->m_tail = NULL;
    list->m_count = 0;
}

int QList_find(QList *list, void *data)
{
    QNode *node;
    assert(list && data);

    node = _ListFindNode(list, data);
    return node ? QLIB_SUCCEEDED : QLIB_ERR_NOTFOUND;
}

int QList_getCount(QList *list)
{
    assert(list);
    return list->m_count;
}

void QListIterReset(QListIterator *iter, QList *list)
{
    assert(iter && list);

    iter->m_list = list;
    iter->m_index = 0;
    iter->m_node = list->m_head;
}

void* QListIterNext(QListIterator *iter)
{
    QNode    *node;
    NodeData *data;
    assert(iter);

    node = iter->m_node;
    data = NULL;
    if(node != NULL){
        iter->m_node = node->m_next;//iter->m_index not used here
        data = node->m_data;
    }
    return data;
}