
#ifndef _QLIB_STACK_H_
#define _QLIB_STACK_H_

#include "qlist.h"
#include "qgeneral.h"

typedef struct QStack_T QStack;

struct QStack_T{
    QList m_list;
};

typedef struct{
    QStack *m_stack;
    QNode  *m_node;
    int m_index;
}QStackIterator;

void  QStack_init(QStack *stack);
void* QStack_pop(QStack *stack);
void  QStack_popAll(QStack *stack);
void  QStack_push(QStack *stack, void *elem);
void* QStack_top(QStack *stack);
int   QStack_getCount(QStack *stack);
int   QStack_empty(QStack *stack);

void  QStackIterReset(QStackIterator *iter, QStack *stack);
void* QStackIterNext(QStackIterator *iter);
#endif //!_QLIB_STACK_H_