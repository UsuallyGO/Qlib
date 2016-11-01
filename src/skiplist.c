
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "qlib/qgeneral.h"
#include "qlib/qskiplist.h"
#include "qlib/qmemory.h"

QSkiplist* QSkiplist_create(DataCompFunc cmp, DataDestroyFunc des)
{
    QSkiplist *slist;

    slist = (QSkiplist*)QMem_Malloc(sizeof(QSkiplist));

    if(slist != NULL)
    {
        slist->m_length = 0;
        slist->m_levels = 0;
        slist->cmpf = cmp;
        slist->desf = des;
    
        slist->m_head = slist->m_tail = NULL;
        memset(slist->m_link, 0, sizeof(slist->m_link));
    }

    return slist;
}

int QSkiplist_insert(QSkiplist *slist, void *data)
{
    QSkiplistNode *node, *cur, *prev;
    QSkiplistNode *node_prev[QSKIPLIST_MAX_LEVELS];
    int index, levels, res;

    if(slist == NULL || data == NULL)
        return QLIB_ERR_INVAL;

    for(index = 0; index < QSKIPLIST_MAX_LEVELS; index++)
        node_prev[index] = &slist->m_link[index];

    for(index = slist->m_levels; index >= 0; index--)
    {
        prev = &slist->m_link[index];
        cur = prev->m_next[0];
        while(cur != NULL && (res = slist->cmpf(cur->m_data, data)) <= 0)//only ascend now
        {
            if(res == 0)//duplicate node in the list
                return QLIB_ERR_DUPLICATE;//just return here...

            prev = cur, cur = cur->m_next[index];
        }

        node_prev[index] = prev;
    }//get the prev of 'node' in each level now

    levels = 0;
    srand(time(0)+rand());
    while(levels < QSKIPLIST_MAX_LEVELS)
    {
        if((rand()%100)%2)
            levels++;
        else
            break;
    }
    if(levels >= QSKIPLIST_MAX_LEVELS)
        levels--;

    node = (QSkiplistNode*)QMem_Malloc(sizeof(QSkiplistNode) + sizeof(QSkiplistNode*)*levels);
    if(node == NULL)
        return QLIB_ERR_NOMEM;
    node->m_data = data;
    memset(node->m_next, 0, sizeof(QSkiplistNode*)*(levels+1));

    for(index = 0; index <= levels; index++)
    {
        if(node_prev[index] == &slist->m_link[index])
        {
            node->m_next[index] = slist->m_link[index].m_next[0];
            slist->m_link[index].m_next[0] = node;
        }
        else
        {
            node->m_next[index] = node_prev[index]->m_next[index];
            node_prev[index]->m_next[index] = node;
        }
    }

    if(levels > slist->m_levels)
        slist->m_levels = levels;
    slist->m_length++;

    if(slist->m_head == node->m_next[0])
        slist->m_head = node;
    if(node->m_next[0] == NULL)
        slist->m_tail = node;

    return QLIB_SUCCEEDED;
}

void QSkiplist_destroy(QSkiplist *slist)
{
    QSkiplistNode *node, *tmp;

    if(slist->desf != NULL)
    {
        node = slist->m_link[0].m_next[0];
        while(node != NULL)
        {
            tmp = node;
            node = node->m_next[0];
            slist->desf(node->m_data);
            QMem_Free(tmp);
        }
    }
    else//no destroy function
    {
        node = slist->m_link[0].m_next[0];
        while(node != NULL)
        {
            tmp = node;
            node = node->m_next[0];
            QMem_Free(tmp);
        }
    }

    QMem_Free(slist);
}

int QSkiplist_find(QSkiplist *slist, void *data)
{
    
}

void QSkiplistPrint(QSkiplist *slist, void (*dataPrint)(void*))
{
    int index, tmp;
    QSkiplistNode *node;

    if(slist == NULL)
        return;

    printf("\nlist length:%d \n", slist->m_length);
    printf("list head:");
    dataPrint(slist->m_head->m_data);
    printf("\nlist tail:");
    dataPrint(slist->m_tail->m_data);
    printf("\n");

    for(index = 0; index <= slist->m_levels; index++)    
    {
        node = &slist->m_link[index];
        node = node->m_next[0];
        printf("\nlevel :%d\n", index);
        while(node != NULL)
        {
            dataPrint(node->m_data);
            for(tmp = 0; tmp <= index; tmp++)
                printf("-");
            printf(">");
            node = node->m_next[index];
        }
    }

    printf("\n");
}
