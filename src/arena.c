// Text type: Unix/LF; TAB=4, tab-to-spaces; Oct.18, 2015 
/**@file arena.c
 * Implements routines defined in qarena.h
 *
 * @author Maxwell.Qin (QinLi qinliok@126.com)
 *
 * @code
 * PACKAGE:   qin
 * REVISION:  1.0
 * COPYRIGHT: Qin-domain
 * HISTORY:
 *     July, 23th, 2016  Rev1.0
 *             - Implement QArena.
 *
 * How sleepy...I'm going to die...Please help me...
 * @endcode
 */

#include <stdlib.h>
#include <assert.h>
#include "qlib/qgeneral.h"
#include "qlib/qatomic.h"
#include "qlib/qarena.h"
#include "qlib/qmemory.h"

/* Ugly code!!! Don't tell anybody this is written by @Maxwell */

QArena* QArena_new()
{
    int index;

    QArena *arena = (QArena*)QMem_Malloc(sizeof(QArena));
    if(arena == NULL)
        goto Arena_Err;

    arena->m_count = (QAtomicPtr*)QMem_Malloc(sizeof(QAtomicPtr));
    if(QAtomicPtr_reset(arena->m_count) != QLIB_SUCCEEDED)
        goto ArenaCount_Err;

    arena->m_array = (void**)QMem_Malloc(sizeof(void*)*QARENA_BOUND_DEFAULT);
    if(arena->m_array == NULL)
        goto ArenaArray_Err;

    arena->m_index = 0;
    arena->m_bound = QARENA_BOUND_DEFAULT;
    arena->m_curptr = NULL;
    arena->m_remaining = 0;
    for(index = 0; index < QARENA_BOUND_DEFAULT; index++)
        arena->m_array[index] = NULL;

    return arena;

ArenaArray_Err:
    free(arena);
ArenaCount_Err:
    free(arena->m_count);
Arena_Err:
    return NULL;
}

static size_t _ArenaUsage(QArena *arena)
{
    return (size_t)QAtomicPtr_load(arena->m_count);
}

static char* _ArenaAllocFall(QArena *arena, size_t size)
{
    char *result;
    size_t gap;

    if(arena->m_index >= arena->m_bound)//increase the arena
    {
        if(arena->m_bound == MEMORY_SIZEMAX)//can't be >=, overflow
            return NULL;//no more space
        else if(arena->m_bound < QARENA_BOUND_VALVE)
            arena->m_bound *= 2;
        else//arena->m_bound > QARENA_BOUND_VALVE
        {
            gap = (MEMORY_SIZEMAX - arena->m_bound) < QARENA_BOUND_VALVE ?
                       (MEMORY_SIZEMAX - arena->m_bound) : QARENA_BOUND_VALVE; 
            arena->m_bound += gap; //make sure arena->m_bound <= MEMORY_SIZEMAX
        }

        arena->m_array = (void**)realloc(arena->m_array, arena->m_bound);
        assert(arena->m_array != NULL);
    }

    if(size < QARENA_BLOCK_SIZE/4)
    {
        result = (char*)QMem_Malloc(QARENA_BLOCK_SIZE);
        assert(result != NULL);//lazy boy...
        arena->m_array[arena->m_index] = result;
        arena->m_curptr = result + size;
        arena->m_remaining = QARENA_BLOCK_SIZE - size;
        arena->m_index++;
        QAtomicPtr_save(arena->m_count,(void*)(_ArenaUsage(arena) + QARENA_BLOCK_SIZE + sizeof(char*))); 
    }
    else
    {
        result = (char*)QMem_Malloc(size);
        assert(result != NULL);
        arena->m_array[arena->m_index] = result;
        arena->m_index++;
        QAtomicPtr_save(arena->m_count,(void*)(_ArenaUsage(arena) + size + sizeof(char*))); 
        /* We don't touch m_curptr and m_remaining */
    }

    return result;
}

char* QArena_allocate(QArena *arena, size_t size)
{
    char* result = NULL;

    if(arena == NULL || size == 0)//can't allocate 0 bytes
        return NULL;

    if(size <= arena->m_remaining)
    {
        result = arena->m_curptr;
        arena->m_curptr += size;
        arena->m_remaining -= size; //can be negative
    }
    else
        result = _ArenaAllocFall(arena, size);//need a new block

    return result;
}

