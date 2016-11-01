
#include <assert.h>
#include <string.h>
#include "qlib/qgeneral.h"

int QVector_Initial(QVector *vector, int type, unsigned int size)
{
    int vecSize;

    assert(vector);

    vecSize = sizeof(QVector);
    memset(vector, 0, vecSize); //vector should be allocted space already
    switch(type)
    {
    case VALUE_TYPE_INT:
        vector->m_elemSize = sizeof(int);
        break;
    case VALUE_TYPE_DOUBLE:
        vector->m_elemSize = sizeof(double);
        break;
    case VALUE_TYPE_CHAR:
        vector->m_elemSize = sizeof(char); //we have no QString now, 2016.1.17
        break;
    case VALUE_TYPE_U32:
        vector->m_elemSize = sizeof(unsigned char[4]);
        break;
    case VALUE_TYPE_U64:
        vector->m_elemSize = sizeof(unsigned char[8]);
        break;
    case VALUE_TYPE_ADT:
        vector->m_elemSize = sizeof(void *);
        break;
    default:
        return QLIB_ERR_INVAL;
    }
    vector->m_dataType = type;

    if(!(size*vector->m_elemSize < MEMORY_SIZEMAX-vecSize))
        return QLIB_ERR_INVAL;
    vector->m_capacity = size;
    vector->m_size = size*vector->m_elemSize;
    vector->m_elemNums = 0;
    vector->m_data = (unsigned char*)malloc(vector->m_size);
    assert(vector->m_data);
    memset(vector->m_data, 0, vector->m_size);

    return vector->m_capacity;//return the capacity not vector->m_size
}

int QVector_Push(QVector *vector, ULL value)
{
    int memLeft;
    assert(vector);

    /* m_elemNums increase from 0, m_elemNums == m_capacity means there is
       space for one more element */
    if(vector->m_elemNums > vector->m_capacity)//space not enough, resize the vector
    {
        if(vector->m_size * 2 < MEMORY_SIZEMAX)
        {
            vector->m_size *= 2; //increased twice
            vector->m_capacity *= 2;
        }
        else
        {
            memLeft = MEMORY_SIZEMAX - vector->m_size;
            memLeft = memLeft / vector->m_elemSize;
            if(memLeft == 0)//no more space for one element
                return QLIB_ERR_NOMEM;
            vector->m_capacity += memLeft;
            vector->m_size += memLeft * vector->m_capacity;
            vector->m_data = (unsigned char*)realloc(vector->m_data, vector->m_size);
        }
    }

    switch(vector->m_dataType)
    {
    case VALUE_TYPE_INT:
        *(int*)&(vector->m_data[vector->m_elemSize*vector->m_elemNums]) = (int)value;
        break;
    case VALUE_TYPE_DOUBLE:
        *(double*)&(vector->m_data[vector->m_elemSize*vector->m_elemNums]) = (double)value;
        break;
    case VALUE_TYPE_CHAR:
        *(char*)&(vector->m_data[vector->m_elemSize*vector->m_elemNums]) = (char)value;
        break;
    case VALUE_TYPE_U32:
        *(UI*)&(vector->m_data[vector->m_elemSize*vector->m_elemNums]) = (UI)value;
        break;
    case VALUE_TYPE_U64:
        *(ULL*)&(vector->m_data[vector->m_elemSize*vector->m_elemNums]) = (ULL)value;
        break;
    case VALUE_TYPE_ADT:
        *(int**)&(vector->m_data[vector->m_elemSize*vector->m_elemNums]) = (int*)value;
        break;
    default:
        return QLIB_ERR_INVAL;
    }
    vector->m_elemNums++;

    return QLIB_SUCCEEDED;
}

//returns 0, means vector is empty
int QVector_ElemCount(QVector *vector)
{
    assert(vector);
    return vector->m_elemNums;
}

static ULL _GetElemByIndex(QVector *vector, int index)
{
    ULL value;

    assert(vector);
    if(!vector->m_elemNums || vector->m_elemNums >= index)
        return NULL;

    switch(vector->m_dataType)
    {
    case VALUE_TYPE_INT:
        value = *(int*)&(vector->m_data[index]);
        break;
    case VALUE_TYPE_DOUBLE:
        value = *(double*)&(vector->m_data[index]);
        break;
    case VALUE_TYPE_CHAR:
        value = *(char*)&(vector->m_data[index]);
        break;
    case VALUE_TYPE_ADT:
        value = *(int**)&(vector->m_data[index]);
        break;
    case VALUE_TYPE_U32:
        value = *(UI*)&(vector->m_data[index]);
        break;
    case VALUE_TYPE_U64:
        value = *(ULL*)&(vector->m_data[index]);
        break;
    default:
        assert(0);//Serious problem
    }
        
    return value;
}

ULL QVector_Front(QVector *vector)
{
    return _GetElemByIndex(vector, 0);
}

ULL QVector_End(QVector *vector)
{
    assert(vector);
    return _GetElemByIndex(vector, vector->m_elemNums-1);
}

int QVector_Get(QVector *vector, ULL value)
{
    assert(vector);

    
}
