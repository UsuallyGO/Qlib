
#include <string.h>
#include "qlib/qgeneral.h"
#include "qlib/qsort.h"
#include "qlib/qmemory.h"

/* This file has been written many duplicate program segments... sorry */
/* Perhaps need more tests here... @_@ */

void _swap(void *data1, void *data2, size_t size)
{
    void *tmp;

    tmp = QMem_Malloc(size);
    memcpy(tmp, data1, size);
    memcpy(data1, data2, size);
    memcpy(data2, tmp, size);
    QMem_Free(tmp);
}

void _selectSort(QSort *sort)
{
    int index1, index2, minOrmax;
    size_t size;
    DataCompFunc cmpf;

    size = sort->m_elemsize;
    cmpf = sort->cmpf;

    //each time, find the minium element, to swap with the ith element
    if(sort->m_order == QSORT_DESCEND)//write the code segment twice, to avoid more judgement in for statement
        for(index1 = sort->m_begin; index1 < sort->m_length-1; index1++)
        {
            minOrmax = index1;
            for(index2 = index1+1; index2 < sort->m_length; index2++)
            {
                if(cmpf(sort->m_data+index2*size, sort->m_data+minOrmax*size) > 0)
                        minOrmax = index2;
            }
            //get the element address accord to the element size
            //we do this because we don't know the element type, can't use sort->m_data[index1]
            _swap(sort->m_data+index1*size, sort->m_data+minOrmax*size, size);
        } 
    else
        for(index1 = sort->m_begin; index1 < sort->m_length-1; index1++)
        {
            minOrmax = index1;
            for(index2 = index1+1; index2 < sort->m_length; index2++)
            {
                if(cmpf(sort->m_data+index2*size, sort->m_data+minOrmax*size) < 0)
                        minOrmax = index2;
            }
            _swap(sort->m_data+index1*size, sort->m_data+minOrmax*size, size);
        }
}

void _insertSort(QSort *sort)
{
    int index1, index2;
    size_t size;
    DataCompFunc cmpf;

    size = sort->m_elemsize;
    cmpf = sort->cmpf;

    if(sort->m_order == QSORT_DESCEND)
        for(index1 = sort->m_begin+1; index1 < sort->m_length; index1++)
        {
            for(index2 = index1; index2 > sort->m_begin; index2--)
            {
                if(cmpf(sort->m_data+index2*size, sort->m_data+(index2-1)*size) > 0)
                    _swap(sort->m_data+index2*size, sort->m_data+(index2-1)*size, size);
            }
        }
    else
        for(index1 = sort->m_begin+1; index1 < sort->m_length; index1++)
        {
            for(index2 = index1; index2 > sort->m_begin; index2--)
            {
                if(cmpf(sort->m_data+index2*size, sort->m_data+(index2-1)*size) < 0)
                    _swap(sort->m_data+index2*size, sort->m_data+(index2-1)*size, size);
            }
        }
}

void _bubbleSort(QSort *sort)
{
    int index1, index2;
    size_t size;
    DataCompFunc cmpf;

    size = sort->m_elemsize;
    cmpf = sort->cmpf;

    if(sort->m_order == QSORT_DESCEND)
        for(index1 = sort->m_begin; index1 < sort->m_length; index1++)
        {
            for(index2 = sort->m_length-1; index2 > index1; index2--)
            {
                if(cmpf(sort->m_data+index2*size, sort->m_data+index1*size) > 0)
                    _swap(sort->m_data+index2*size, sort->m_data+index1*size, size);
            }
        }
    else
        for(index1 = sort->m_begin; index1 < sort->m_length; index1++)
        {
            for(index2 = sort->m_length-1; index2 > index1; index2--)
            {
                if(cmpf(sort->m_data+index2*size, sort->m_data+index1*size) < 0)
                    _swap(sort->m_data+index2*size, sort->m_data+index1*size, size);
            }
        }
}

void _quick_sort_descend(void *data, int begin, int end, size_t size, DataCompFunc cmpf)
{
    unsigned char *sentinel;//we don't know the element type
    int first, middle, last;

    if(begin >= end)
        return;

    first = begin;
    last = end;
    middle = (first + last)/2;

    //put the middle one to first place
    if(cmpf(data+first*size, data+middle*size) > 0)//first > middle
    {
        if(cmpf(data+first*size, data+last*size) > 0)//first > last, first is the largest
        {
            if(cmpf(data+middle*size, data+last*size) > 0)//middle > last, first > middle > last
                _swap(data+first*size, data+middle*size, size);//last<first<middle
            else //middle < last, first > last > middle
                _swap(data+first*size, data+last*size, size);//middle<first<last
        }
        else //first <= last, middle < first < last
            ;//need to do nothing here
    }
    else //first <= middle
    {
        if(cmpf(data+middle*size, data+last*size) > 0)//middle > last, middle is the largest
        {
            if(cmpf(data+first*size, data+last*size) > 0)//first>last, now is middle>first>last
                ;//nothing has to be done here, last<first<middle
        }
        else //middle <= last, first < middle < last
            _swap(data+first*size, data+middle*size, size);//middle<first<last
    }

    //first is the middle one
    sentinel = QMem_Malloc(size);//safety memory allocate
    memcpy(sentinel, data+first*size, size);//restore the sentinel

    while(first != last)
    {
        while(last>first && cmpf(data+last*size, sentinel) < 0)
            last--;//step backward
        if(first < last)
        {
            memcpy(data+first*size, data+last*size, size);
            first++;
        }

        while(first<last && cmpf(data+first*size, sentinel) > 0)
            first++; //step forward
        if(first < last)
        {
             memcpy(data+last*size, data+first*size, size);
             last--;
        }
    }

    memcpy(data+first*size, sentinel, size);
    QMem_Free(sentinel);

    _quick_sort_descend(data, begin, first-1, size, cmpf);
    _quick_sort_descend(data, first+1, end, size, cmpf);
}

void _quick_sort_ascend(void *data, int begin, int end, size_t size, DataCompFunc cmpf)
{
    unsigned char *sentinel;//we don't know the element type
    int first, middle, last;

    if(begin >= end)
        return;

    first = begin;
    last = end;
    middle = (first + last)/2;

    //put the middle one to first place
    if(cmpf(data+first*size, data+middle*size) > 0)//first > middle
    {
        if(cmpf(data+first*size, data+last*size) > 0)//first > last, first is the largest
        {
            if(cmpf(data+middle*size, data+last*size) > 0)//middle > last, first > middle > last
                _swap(data+first*size, data+middle*size, size);//last<first<middle
            else //middle < last, first > last > middle
                _swap(data+first*size, data+last*size, size);//middle<first<last
        }
        else //first <= last, middle < first < last
            ;//need to do nothing here
    }
    else //first <= middle
    {
        if(cmpf(data+middle*size, data+last*size) > 0)//middle > last, middle is the largest
        {
            if(cmpf(data+first*size, data+last*size) > 0)//first>last, now is middle>first>last
                ;//nothing has to be done here, last<first<middle
        }
        else //middle <= last, first < middle < last
            _swap(data+first*size, data+middle*size, size);//middle<first<last
    }

    //first is the middle one
    sentinel = QMem_Malloc(size);//safety memory allocate
    memcpy(sentinel, data+first*size, size);//restore the sentinel

    while(first != last)
    {
        while(last>first && cmpf(data+last*size, sentinel) > 0)
            last--;//step backward
        if(first < last)
        {
            memcpy(data+first*size, data+last*size, size);
            first++;
        }

        while(first<last && cmpf(data+first*size, sentinel) < 0)
            first++; //step forward
        if(first < last)
        {
             memcpy(data+last*size, data+first*size, size);
             last--;
        }
    }

    memcpy(data+first*size, sentinel, size);
    QMem_Free(sentinel);

    _quick_sort_ascend(data, begin, first-1, size, cmpf);
    _quick_sort_ascend(data, first+1, end, size, cmpf);
}

void _quickSort(QSort *sort)
{
    int begin, end;
    size_t size;
    DataCompFunc cmpf;

    size = sort->m_elemsize;
    cmpf = sort->cmpf;

    begin = sort->m_begin;
    end = sort->m_length - 1;

    if(sort->m_order == QSORT_DESCEND)
        _quick_sort_descend(sort->m_data, begin, end, size, cmpf);
    else
        _quick_sort_ascend(sort->m_data, begin, end, size, cmpf);
}

void _shellSort(QSort *sort)
{
    int begin, end, interval;
    int index, index2;
    size_t size;
    DataCompFunc cmpf;
    void *tmp;

    size = sort->m_elemsize;
    cmpf = sort->cmpf;

    begin = sort->m_begin;
    end = sort->m_length - 1;

   tmp = QMem_Malloc(size);

    if(sort->m_order == QSORT_DESCEND)
        for(interval = (end-begin+1)/2; interval > 0; interval--)
        {
            for(index = interval; index <= end; index++)
            {
                //this judgement make sure that memcpy will be done at least once
                if(cmpf(sort->m_data+index*size, sort->m_data+(index-interval)*size) > 0)
                {
                    memcpy(tmp, sort->m_data+index*size, size);
                    index2 = index - interval;
                    while(index2 >= begin &&
                        cmpf(sort->m_data+index*size, sort->m_data+index2*size) > 0)
                    {
                        memcpy(sort->m_data+(index2+interval)*size, sort->m_data+index2*size, size);
                        index2 -= interval;
                    }
                    memcpy(sort->m_data+(index2+interval)*size, tmp, size);
                }
            }
        }
    else//ascend
        for(interval = (end-begin+1)/2; interval > 0; interval--)
        {
            for(index = interval; index <= end; index++)
            {
                //this judgement make sure that memcpy will be done at least once
                if(cmpf(sort->m_data+index*size, sort->m_data+(index-interval)*size) < 0)
                {
                    memcpy(tmp, sort->m_data+index*size, size);
                    index2 = index - interval;
                    while(index2 >= begin &&
                        cmpf(sort->m_data+index*size, sort->m_data+index2*size) < 0)
                    {
                        memcpy(sort->m_data+(index2+interval)*size, sort->m_data+index2*size, size);
                        index2 -= interval;
                    }
                    memcpy(sort->m_data+(index2+interval)*size, tmp, size);
                }
            }
        }

    QMem_Free(tmp);
}

void _merge_descend(void *data, void *tmp, int begin, int mid, int end,
                           size_t size, DataCompFunc cmpf)
{
    int index1, index2;
    int label;

    index1 = begin;
    index2 = mid+1;
    label = 0;

    while(index1 <= mid && index2 <= end)
    {
        if(cmpf(data+index1*size, data+index2*size) < 0)
        {
            memcpy(tmp+label*size, data+index2*size, size);
            index2++, label++;
        }
        else
        {
            memcpy(tmp+label*size, data+index1*size, size);
            index1++, label++;
        }
    }

    while(index1 <= mid)
    {
        memcpy(tmp+label*size, data+index1*size, size);
        index1++, label++;
    }
    while(index2 <= end)
    {
        memcpy(tmp+label*size, data+index2*size, size);
        index2++, label++;
    }

    for(index1 = 0; index1 < label; index1++)
        memcpy(data+(begin+index1)*size, tmp+index1*size, size);
}

void _merge_ascend(void *data, void *tmp, int begin, int mid, int end,
                         size_t size, DataCompFunc cmpf)
{
    int index1, index2;
    int label;

    index1 = begin;
    index2 = mid+1;
    label = 0;

    while(index1 <= mid && index2 <= end)
    {
        if(cmpf(data+index1*size, data+index2*size) > 0)
        {
            memcpy(tmp+label*size, data+index2*size, size);
            index2++, label++;
        }
        else
        {
            memcpy(tmp+label*size, data+index1*size, size);
            index1++, label++;
        }
    }

    while(index1 <= mid)
    {
        memcpy(tmp+label*size, data+index1*size, size);
        index1++, label++;
    }
    while(index2 <= end)
    {
        memcpy(tmp+label*size, data+index2*size, size);
        index2++, label++;
    }

    for(index1 = 0; index1 < label; index1++)
        memcpy(data+(begin+index1)*size, tmp+index1*size, size);
}

void _merge_sort(void *data, void *tmp, int begin, int end, size_t size,
                 DataCompFunc cmpf, int order)
{
    int mid;

    if(begin < end)
    {
        mid = (begin+end)/2;
        _merge_sort(data, tmp, begin, mid, size, cmpf, order);
        _merge_sort(data, tmp, mid+1, end, size, cmpf, order);
        if(order == QSORT_DESCEND)
            _merge_descend(data, tmp, begin, mid, end, size, cmpf);
        else
            _merge_ascend(data, tmp, begin, mid, end, size, cmpf);
    }
}

void _mergeSort(QSort *sort)
{
    int begin, end;
    size_t size;
    DataCompFunc cmpf;
    void *tmp;

    size = sort->m_elemsize;
    cmpf = sort->cmpf;

    begin = sort->m_begin;
    end = sort->m_length - 1;

    tmp = QMem_Malloc(size*sort->m_length);//memory complexity of merge sort is O(n)
    _merge_sort(sort->m_data, tmp, begin, end, size, cmpf, sort->m_order);
    QMem_Free(tmp);
}

void _heap_fixup_ascend(void *data, int begin, int end, size_t size, DataCompFunc cmpf)
{
    int index1, index2;

    index1 = begin;
    index2 = index1*2 + 1;//left son of data[index1] is data[index*2+1]
    while(index2 <= end)//we will use index2+1, so index2 must be less than end
    {
        if(index2 < end && cmpf(data+index2*size, data+(index2+1)*size) > 0)
            index2++;//get the less one 

        if(cmpf(data+index1*size, data+index2*size) > 0)
        {
            _swap(data+index1*size, data+index2*size, size);
            index1 = index2;
            index2 = index1*2 + 1;
        }
        else
            break;
    }
}

void _heap_fixup_descend(void *data, int begin, int end, size_t size, DataCompFunc cmpf)
{
    int index1, index2;

    index1 = begin;
    index2 = index1*2 + 1;//left son of data[index1] is data[index*2+1]
    while(index2 <= end)//we will use index2+1, so index2 must be less than end
    {
        if(index2 < end && cmpf(data+index2*size, data+(index2+1)*size) < 0)
            index2++;//get the bigger one

        if(cmpf(data+index1*size, data+index2*size) < 0)
        {   //set the bigger son to the root
            _swap(data+index1*size, data+index2*size, size);
            index1 = index2;
            index2 = index1*2 + 1;
        }
        else
            break;
    }
}


void _heapSort(QSort *sort)
{
    int begin, end;
    size_t size;
    DataCompFunc cmpf;
    void *tmp;

    size = sort->m_elemsize;
    cmpf = sort->cmpf;

    //Make sure the last one is the biggest or least one
    begin =(sort->m_length - 1)/2;
    end = sort->m_length - 1;
    if(sort->m_order == QSORT_DESCEND)
        for( ; begin >= 0; begin--)
            _heap_fixup_descend(sort->m_data, begin, end, size, cmpf);
    else
        for( ; begin >=0; begin--)
            _heap_fixup_ascend(sort->m_data, begin, end, size, cmpf);

    begin = sort->m_begin;
    if(sort->m_order == QSORT_DESCEND)
        for( ; end >= 0; end--)
        {
            _swap(sort->m_data+begin*size, sort->m_data+end*size, size);
            _heap_fixup_descend(sort->m_data, begin, end-1, size, cmpf);
        }
    else
        for( ; end >= 0; end--)
        {
            _swap(sort->m_data+begin*size, sort->m_data+end*size, size);
            _heap_fixup_ascend(sort->m_data, begin, end-1, size, cmpf);
        }     
}

void QSort_sort(QSort *sort)
{
    if(sort == NULL)
        return;

    if(sort->m_order != QSORT_DESCEND && sort->m_order != QSORT_ASCEND)
        sort->m_order = QSORT_ASCEND;

    if(sort->m_sortway < 0 || sort->m_sortway >= QSORT_SORT_DEFAULT)
        sort->m_sortway = QSORT_SORT_SELECT, sort->sortf = _selectSort;

    sort->sortf(sort);
}

void QSort_config(QSort *sort, int sortOrder, int sortWay)
{
    if(sort == NULL)
        return;

    sort->m_order = sortOrder;
    if(sortOrder != QSORT_DESCEND)
        sort->m_order = QSORT_ASCEND;//default is ascend, bigger and bigger

    sort->m_sortway = sortWay;
    switch(sort->m_sortway)
    {
    case QSORT_SORT_SELECT:
        sort->sortf = _selectSort;
        break;
    case QSORT_SORT_INSERT:
        sort->sortf = _insertSort;
        break;
    case QSORT_SORT_BUBBLE:
        sort->sortf = _bubbleSort;
        break;
    case QSORT_SORT_QUICK:
        sort->sortf = _quickSort;
        break;
    case QSORT_SORT_SHELL:
        sort->sortf = _shellSort;
        break;
    case QSORT_SORT_MERGE:
        sort->sortf = _mergeSort;
        break;
    case QSORT_SORT_HEAP:
        sort->sortf = _heapSort;
        break;
    default:
        sort->sortf = _selectSort;
        sort->m_sortway = QSORT_SORT_SELECT;
    }

}

void QSort_set(QSort *sort, void *data, int begin, int length, 
                    size_t size, DataCompFunc cmp)
{
    sort->m_data = data;
    sort->m_begin = begin;
    sort->m_length = length;
    sort->m_elemsize = size;

    sort->cmpf = cmp;
}
