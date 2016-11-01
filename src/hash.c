
#include <limits.h> //for CHAR_BIT
#include <string.h> // strlen(), memset()
#include <math.h>   // pow()
#include <stdio.h> //stderr
#include <stdlib.h> //exit()
#include <assert.h> //assert()
#include "qlib/qhash.h"
#include "qlib/qmemory.h"
#include "qlib/qstring.h"
#include "qlib/qgeneral.h"

/*More details about these hash functions please read the comments in qhash.h.*/

unsigned int QHash_PJW(const char *datum)
{
    const int BITS_IN_int   = sizeof(int)*CHAR_BIT; //CHAR_BIT is defined in limists.h
    const int THREE_QUATERS = (int)((BITS_IN_int*3)/4);
    const int ONE_EIGHTH    = (int)(BITS_IN_int/8);
    const int HIGH_BITS     = (int)(~((unsigned int)(~0) >> ONE_EIGHTH));

    unsigned int hash, index;

    for(hash = 0; *datum; ++datum)
    {
        hash = (hash << ONE_EIGHTH) + *datum;
        if((index = hash&HIGH_BITS) != 0)
            hash = (hash^(index >> THREE_QUATERS))&(~HIGH_BITS);
    }
    return hash;
}

unsigned int QHash_ELF(const char *datum)
{
    unsigned int hash = 0, tmp;

    while(*datum)
    {
        hash = (hash << 4) + *datum++;
        if(tmp = hash & 0xF0000000)
            hash ^= tmp >> 24;
        hash &= ~tmp;
    }

    return hash;
}

unsigned int QHash_BKDR(const char *datum)
{
    unsigned int seed = 131; //or 31 131 1313 13131 131313 etc.
    unsigned int hash = 0;
    const UL M = ((~0) >> 1);

    while(*datum)
        hash = hash*seed + (*datum++);

    return (hash & M);
}

//Feel odd about the 7, 3, 11 and 5?
//Read:http://www.partow.net/programming/hashfunctions/index.html
unsigned int QHash_AP(const char *datum)
{
    unsigned int hash = 0;
    const UL M = ((~0) >> 1);
    int index;

    for(index = 0; *datum; index++)
    {
        if((index % 2) == 0)
            hash ^= ((hash << 7)^(*datum++)^(hash >> 3));
        else
            hash ^= (~((hash << 11)^(*datum++)^(hash >> 5)));
    }

    return (hash & M);
}

unsigned int QHash_DEK(const char *datum)
{
    unsigned int hash = strlen(datum);
    const UL M = ((~0) >> 1);

    while(*datum)
        hash = ((hash << 5)^(hash >> 27))^(*datum++);

    return (hash & M);
}

static hashFunc hashFuncArr[] = { QHash_PJW, QHash_ELF, QHash_BKDR,
                                  QHash_AP,  QHash_DEK };

int QHashTable_create(QHashTable *table, int dataType, int hashFunc, 
                      int colDeal, valueDesFunc vdesFunc, unsigned int size)
{
    unsigned int table_size;
    assert(table && size); //size can't be zero

    if(dataType < VALUE_TYPE_INT || dataType > VALUE_TYPE_U64)
        return QLIB_ERR_INVAL;
    if(hashFunc < HASH_PJW_FUNC || hashFunc > HASH_DEK_FUNC)
        return QLIB_ERR_INVAL;
    if(colDeal < HASH_LINEAR_REHASHING || colDeal > HASH_EXTERNAL_CHAIN)
        return QLIB_ERR_INVAL;

    table_size = (unsigned int)pow(2.0, size); //2^size
    table = (QHashTable*)QMem_Malloc(sizeof(QHashTable) + 
                  (table_size-1)*sizeof(QHashElement*));
    if(table == NULL)
        return QLIB_ERR_NOMEM;

    table->m_capacity = table_size;
    table->m_counter  = 0;
    table->m_dataType = dataType;
    table->m_colDeal  = colDeal;
    table->m_hashFunc = hashFuncArr[hashFunc];
    table->m_vdesFunc = vdesFunc;
    memset(table->m_value, 0, table_size*sizeof(QHashElement*));
    return QLIB_SUCCEEDED;
}

//This function is not called by user directly, value is 64bits so can convert
//to any other data type whether on 32bits or 64bits machine
int QHashTable_seT(QHashTable *table, const char *key, ULL value)
{
    double loadFactor;
    unsigned int index;
    UL  hashKey, rehashKey;
    QHashElement *element, *tmp;
    assert(table && key);

    loadFactor = (double)table->m_counter/(double)table->m_capacity;
    if(loadFactor >= 0.8500) //occupied elements should not more than 85% percent 
        return QLIB_ERR_DONE;

    hashKey = (table->m_hashFunc)(key);
    element = (QHashElement*)QMem_Malloc(sizeof(QHashElement));
    if(element == NULL)
        return QLIB_ERR_NOMEM;

    element->m_key  = QStrdup(key);
    element->m_next = NULL;
    switch(table->m_dataType)
    {
    case VALUE_TYPE_INT:
        (element->m_value).m_int = (int)value;
        break;
    case VALUE_TYPE_STRING:
		
#ifdef __GNUC__
  #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif
        (element->m_value).m_str = QStrdup((const char*)value);
#ifdef __GNUC__
  #pragma GCC diagnostic warning "-Wint-to-pointer-cast"
#endif
        break;

    case VALUE_TYPE_DOUBLE:
        (element->m_value).m_double  = (double)value;
        break;
    case VALUE_TYPE_UDEFINE:
		
#ifdef __GNUC__
  #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif
        (element->m_value).m_udefine = (UDefType)value; //warning will be cast on 32bits machine
#ifdef __GNUC__
  #pragma GCC diagnostic warning "-Wint-to-pointer-cast"
#endif
        break;

    case VALUE_TYPE_U32:
        (element->m_value).m_u32 = (UL)value;
        break;
    case VALUE_TYPE_U64:
        (element->m_value).m_u64 = (ULL)value;
        break;
    default:
        fprintf(stderr, "Hash fatal error: error data type...\n");
        exit(1); //Perhaps should be abort()?
    }
    if(table->m_value[hashKey] == NULL)
        table->m_value[hashKey] = element;
    else if(table->m_colDeal == HASH_LINEAR_REHASHING)
    {
        for(index = 1; index < table->m_capacity; index++)
        {
            hashKey = (hashKey + index)%table->m_capacity;
            if(table->m_value[hashKey] == NULL)
            {
                table->m_value[hashKey] = element;
                break;
            }
        }
        if(index == table->m_capacity)//Cannot be here...
        {
            fprintf(stderr, "Hash fatal error: unknow error...\n");
            exit(1);
        }
    }
    else if(table->m_colDeal == HASH_NONLINEAR_REHASHING)
    {
        srand(hashKey);
        while(1)
        {
            rehashKey = rand() % table->m_capacity;
            if(table->m_value[rehashKey] == NULL)
                break;
        }
        table->m_value[rehashKey] = element;
    }
    else if(table->m_colDeal == HASH_EXTERNAL_CHAIN)
    {
        tmp = table->m_value[hashKey];
        while(tmp->m_next != NULL)
            tmp = tmp->m_next;
        tmp->m_next = element;
    }
    else
    {
        fprintf(stderr, "Hash fatal error: error collision deal function...\n");
        exit(1);
    }
    table->m_counter++;
    return QLIB_SUCCEEDED;
}

int QHashTable_get(QHashTable *table, const char *key, void *value)
{
    unsigned int index;
    unsigned int  hashKey, rehashKey;
    QHashElement *element;
    assert(table && key && value);

    hashKey = (table->m_hashFunc)(key);
    element = table->m_value[hashKey];
    if(element == NULL)
        return QLIB_ERR_NOTFOUND;
    if(strcmp(element->m_key, key))//collision happened
    {
        if(table->m_colDeal == HASH_LINEAR_REHASHING)
        {
            for(index = 1; index < table->m_capacity; index++)
            {
                hashKey = (hashKey+index)%table->m_capacity;
                element = table->m_value[hashKey];
                if(element == NULL || !strcmp(element->m_key, key))
                    break;
            }
            if(element == NULL || index == table->m_capacity)
                return QLIB_ERR_NOTFOUND;
        }
        else if(table->m_colDeal == HASH_NONLINEAR_REHASHING)
        {
            srand(hashKey);
            while(1)
            {
                rehashKey = rand()%table->m_capacity;
                element = table->m_value[rehashKey];
                if(element == NULL || !strcmp(element->m_key, key))
                    break; //serious bug here
            }
            if(element == NULL)
                return QLIB_ERR_NOTFOUND;
        }
        else if(table->m_colDeal == HASH_EXTERNAL_CHAIN)
        {
            element = element->m_next; 
            for(; element != NULL; element = element->m_next)
                if(!strcmp(element->m_key, key))
                    break;
            if(element == NULL)
                return QLIB_ERR_NOTFOUND;
        }
        else
        {
            fprintf(stderr, "Hash fatal error: error collision deal function...\n");
		    exit(1);
        }
    }

	//element has been found
    switch(table->m_dataType)
    {
    case VALUE_TYPE_INT:
        *(int*)value = (element->m_value).m_int;
        break;
    case VALUE_TYPE_STRING:
        *(char**)value = (element->m_value).m_str; 
        break;
    case VALUE_TYPE_DOUBLE:
        *(double*)value = (element->m_value).m_double;
        break;
    case VALUE_TYPE_UDEFINE:
        *(UDefType*)value = (element->m_value).m_udefine;
        break;
    case VALUE_TYPE_U32:
        *(UL*)value = (element->m_value).m_u32;
        break;
    case VALUE_TYPE_U64:
        *(ULL*)value = (element->m_value).m_u64;
        break;
    default:
        fprintf(stderr, "Hash fatal error: error data type...\n");
        exit(1);
    }
    return QLIB_SUCCEEDED;
}

static void _ElementDestroy(QHashTable *table, QHashElement *element)
{
    if(table != NULL && element != NULL)
    {
        if(table->m_dataType == VALUE_TYPE_STRING)
            QMem_Free((element->m_value).m_str);
        else if(table->m_dataType == VALUE_TYPE_UDEFINE)
            if(table->m_vdesFunc == NULL)
            {
                fprintf(stderr, "Hash fatal error: user define" 
                        "value type but there is no destroy function...\n");
                exit(1);
            }
            else
                table->m_vdesFunc((element->m_value).m_udefine);
        else;
        QMem_Free(element->m_key);
        QMem_Free(element);
    }
}

int QHashTable_delete(QHashTable *table, const char *key)
{
    unsigned int index;
    QHashElement *element, *tmp;
    unsigned int  hashKey, rehashKey;
    assert(table && key);

    hashKey = (table->m_hashFunc)(key);
    element = table->m_value[hashKey];
    if(element == NULL || table->m_counter == 0)
        return QLIB_ERR_DONE;

    if(table->m_colDeal == HASH_LINEAR_REHASHING)
    {
        if(strcmp(element->m_key, key))
        {
            for(index = 1; index < table->m_capacity; index++)
            {
                hashKey = (hashKey + index) % table->m_capacity;
                element = table->m_value[hashKey];
                if(element == NULL || !strcmp(element->m_key, key))
                    break;
            }
            if(element == NULL || index == table->m_capacity)
                return QLIB_ERR_NOTFOUND;
        }
        //element find
        table->m_value[hashKey] = NULL;
    }
    else if(table->m_colDeal == HASH_NONLINEAR_REHASHING)
    {
        if(strcmp(element->m_key, key))
        {
            srand(hashKey);
            while(1)
            {
                rehashKey = rand()%table->m_capacity;
                element = table->m_value[rehashKey];
                if(element == NULL || !strcmp(element->m_key, key))
                    break; //serious bug here
            }
            if(element == NULL)
                return QLIB_ERR_NOTFOUND;
        }
        table->m_value[rehashKey] = NULL;
    }
    else if(table->m_colDeal == HASH_EXTERNAL_CHAIN)
    {
        if(strcmp(element->m_key, key))
        {
            tmp = element;
            element = element->m_next; 
            for(; element != NULL; element = element->m_next)
            {
                if(!strcmp(element->m_key, key))
                    break;
                tmp = element;
            }
            if(element == NULL)
                return QLIB_ERR_NOTFOUND;
            tmp->m_next = element->m_next;
        }
        else
            table->m_value[hashKey] = element->m_next;
    }
    else
    {
        fprintf(stderr, "Hash fatal error: error collision deal function...\n");
        exit(1);
    }

    //finally, we remove the element from the hash table
    _ElementDestroy(table, element);
    table->m_counter--;
    return QLIB_SUCCEEDED;
}

void QHashTable_clear(QHashTable *table)
{
    unsigned int index;
    QHashElement *element, *tmp;
    assert(table);
    if(table->m_counter == 0)
        return;

    if(table->m_colDeal == HASH_LINEAR_REHASHING
       || table->m_colDeal == HASH_NONLINEAR_REHASHING)
    {
        for(index = 0; index < table->m_capacity; index++)
        {
		    element = table->m_value[index];
            _ElementDestroy(table, element);
            table->m_value[index] = NULL;
        }
    }
    else if(table->m_colDeal == HASH_EXTERNAL_CHAIN)
    {
        for(index = 0; index < table->m_capacity; index++)
        {
            element = table->m_value[index];
            while(element != NULL)
            {
                tmp = element;
                element = element->m_next;
                _ElementDestroy(table, tmp);
            }
            table->m_value[index] = NULL;
        }
    }
    else
    {
        fprintf(stderr, "Hash fatal error: error collision deal function...\n");
        exit(1);
    }

    table->m_counter = 0;
}

void QHashTable_destroy(QHashTable *table)
{
    assert(table);
    QHashTable_clear(table);
    QMem_Free(table); 
}

void QHashTableDefaultValDes(QHashTable *table, void *value)
{
    if(table->m_dataType == VALUE_TYPE_STRING)
        QMem_Free(((QHashValue*)value)->m_str);
}