      
#include <stdlib.h> //malloc()
#include <stddef.h> //size_t
#include <assert.h> //assert()
#include <stdio.h>
#include "qlib/qexception.h"

typedef void* _MemoryPtr;
typedef struct MemorySuite_T MemorySuite;

struct MemorySuite_T
{
    unsigned int m_size;
    _MemoryPtr   m_block;
    unsigned int m_id;
    int          m_line;
    const char*  m_file;
    MemorySuite* m_next;
};

static struct MemorySuite_T* memList = NULL;
static unsigned int memCount = 0;
const Except_Msg Mem_AllocFailed = { "Memory Allocate Failed" };
const Except_Msg Mem_FreeFailed  = { "Free Unknow Memory Block" };

static void SetMemorySuite(MemorySuite *suite, _MemoryPtr *ptr, 
                        unsigned int size, int line, const char *file)
{
    suite->m_size  = size;
    suite->m_block = ptr;
    suite->m_line  = line;
    suite->m_file  = file;
    //should be lock here: Lock.On()
    suite->m_id    = memCount++;
    suite->m_next  = memList;
    memList = suite;
    //release the lock here: Lock.Off()
}

static MemorySuite* RemoveMemorySuite(_MemoryPtr *ptr)
{
    MemorySuite *prev, *cur;

    prev = NULL;
    //should be lock here: Lock.On()
    for(cur = memList; cur != NULL; cur = cur->m_next)
    {
        if(cur->m_block == ptr)
            break;
        prev = cur;
    }

    if(cur != NULL)
    {
        if(cur == memList)
            memList = memList->m_next;
        else
            prev->m_next = cur->m_next;
    }
    //release the lock here: Lock.Off()
    return cur;
}

void* QMem_malloc(unsigned int size, int line, const char *file)
{
    _MemoryPtr   newBlock = NULL;
    MemorySuite *newSuite = NULL;

    newSuite = (MemorySuite*)malloc(sizeof(MemorySuite));
    if(newSuite == NULL)
        return NULL;

    newBlock = (_MemoryPtr)malloc(size);
    if(newBlock == NULL)
    {
        free(newSuite);
        return NULL;
    }

    SetMemorySuite(newSuite, newBlock, size, line, file);
    return newBlock;    
}

//Nerver returns NULL, has been asserted...
void* QMem_realloc(void *ptr, unsigned int size, int line, const char* file)
{
    MemorySuite *cur;

    //Lock here: Lock.On()
    for(cur = memList; cur != NULL; cur = cur->m_next)
    {
        if(cur->m_block == ptr)
            break;
    }
    //Unlock here: Lock.Off();
 
    if(cur != NULL)//we don't care cur is the head or tail of memList
    {
        ptr = (char*)realloc(ptr, size);
        /* Normally, if ptr is NULL, we will miss the original memory address
         * in ptr, which can lead memory leak. Here, we can still find it 
         * in cur->m_block. However, should we just return NULL or cur->m_block?
         * I can't make a decision here, so I choose to assert...
         * This can be modified depend on your usage.
         */
        assert(ptr != NULL);
        cur->m_block = ptr;
        cur->m_size  = size;
        cur->m_line  = line;
        cur->m_file  = file;
    }
    else//cur == NULL, ptr is not allocated by QMem_Malloc, refuse it
        assert(0);

    return ptr;
}

void QMem_free(void* ptr)
{
    MemorySuite *suite;

    if(ptr != NULL)
    {
        suite = RemoveMemorySuite(ptr);
        assert(suite);
        free(ptr);
        free(suite);
    }
}

void* QExc_malloc(unsigned int size, int line, const char *file)
{
    _MemoryPtr   newBlock;
    MemorySuite *newSuite;

    newSuite = (MemorySuite*)malloc(sizeof(MemorySuite));
    if(newSuite == NULL) //program routine will jump to setjmp
        Except_raise(&Mem_AllocFailed, line, file);
	
    newBlock = (_MemoryPtr)malloc(size);
    if(newBlock == NULL)
    {
        free(newSuite); 
        Except_raise(&Mem_AllocFailed, line, file);
    }
	
    SetMemorySuite(newSuite, newBlock, size, line, file);
    return newBlock;
}

void QExc_free(void *ptr, int line, const char* file)
{
    MemorySuite *suite;

    if(ptr == NULL)
        Except_raise(&Mem_FreeFailed, line, file);

    suite = RemoveMemorySuite(ptr);
    if(suite == NULL)
        Except_raise(&Mem_FreeFailed, line, file);

    free(ptr);
    free(suite);
}

void QMemDumpInfo(unsigned int since)
{
    MemorySuite *suite;
    unsigned int count = 0;

    printf("\n");
    //lock memList here
    for(suite = memList; suite != NULL; suite = suite->m_next)
    {
        if(suite->m_id >= since)
        {
            printf("Memory leak, block id %d created at %s, %d line.\n",
                suite->m_id, suite->m_file, suite->m_line);
            count++;
        }
    }
    //release lock memList here
    printf("Memory leak check, %d blocks detected.\n", count);
}

/////////////////////////////////////////////////////////////////////////
//GNU __builtin_alloca() alloc memory space in stack. How to check the 
//stack size of a process? ulimit -a/-s
//