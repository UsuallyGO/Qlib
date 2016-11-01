
#ifndef _QLIB_SORT_H_
#define _QLIB_SORT_H_

#include <stddef.h>
#include "qlib/qgeneral.h"

#define QSORT_ASCEND  0
#define QSORT_DESCEND 1

#define QSORT_SORT_SELECT  0
#define QSORT_SORT_INSERT  1
#define QSORT_SORT_BUBBLE  2
#define QSORT_SORT_QUICK   3
#define QSORT_SORT_SHELL   4
#define QSORT_SORT_MERGE   5
#define QSORT_SORT_HEAP    6
#define QSORT_SORT_DEFAULT 7

typedef struct QSort_T QSort;
typedef void (*QSortFunc)(QSort *);

struct QSort_T
{
    int m_order;
    int m_sortway;

    void *m_data;
    int m_begin;
    int m_length;
    size_t m_elemsize;

    QSortFunc sortf;
    DataCompFunc cmpf;
};

void QSort_config(QSort *sort, int sortOrder, int sortWay);
void QSort_sort(QSort *sort);
void QSort_set(QSort *sort, void *data, int begin, int length, 
                    size_t size, DataCompFunc cmp);

#endif //!_QLIB_SORT_H_