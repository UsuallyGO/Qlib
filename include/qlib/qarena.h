
#ifndef _QLIB_ARENA_H_
#define _QLIB_ARENA_H_

#include "qlib/qatomic.h"

#define QARENA_BLOCK_SIZE    4096
#define QARENA_BOUND_DEFAULT 1024
#define QARENA_BOUND_VALVE   16384

typedef struct Arena_T QArena;

struct Arena_T
{
    int    m_index;
    int    m_bound;
    void*  m_curptr;
    size_t m_remaining;
    void** m_array;

    QAtomicPtr *m_count;//estimate the usage of this Arena
};

QArena* QArena_new();
int QArena_init(QArena *arn);

#endif //!__QLIB_ARNE_H__
