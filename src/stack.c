
#include <assert.h> //assert
#include <stdio.h> //for NULL
#include "qlib/qlist.h"
#include "qlib/qstack.h"

void QStack_init(QStack *stack)
{
    QList_init(&stack->m_list, NULL, NULL);
}

void* QStack_pop(QStack *stack)
{
    void *data;
    assert(stack);

    data = QList_removeHead(&stack->m_list);
    return data;
}

void QStack_popAll(QStack *stack)
{
    assert(stack);

    QList_clearAll(&stack->m_list);
}

void QStack_push(QStack *stack, void *elem)
{
    assert(stack);
    QList_addHead(&stack->m_list, elem);
}

void* QStack_top(QStack *stack)
{
    QNode *node;
    assert(stack);

    if(QList_getCount(&stack->m_list) == 0)
        return NULL;

    node = stack->m_list.m_head;
    return node->m_data;
}

int QStack_getCount(QStack *stack)
{
    assert(stack);

    return QList_getCount(&stack->m_list);
}

int QStack_empty(QStack *stack)
{
    assert(stack);

    if(QList_getCount(&stack->m_list) == 0)
        return 1;
    else
        return 0;
}

void QStackIterReset(QStackIterator *iter, QStack *stack)
{
    assert(iter && stack);

    iter->m_stack = stack;
    iter->m_index = 0;
    iter->m_node = stack->m_list.m_head;
}

void* QStackIterNext(QStackIterator *iter)
{
    void  *data;
    QNode *node;
    assert(iter);

    node = iter->m_node;
    data = NULL;
    if(node != NULL){
        iter->m_node = node->m_next;//iter->m_index not used here
        data = node->m_data;
    }
    return data;        
}
