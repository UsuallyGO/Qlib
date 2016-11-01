// Text type: Unix/LF; TAB=4, tab-to-spaces; Oct.18, 2015 
/**@file atomic.c
 * Implements routines defined in qatomic.h
 *
 * @author Maxwell.Qin (QinLi qinliok@126.com)
 *
 * @code
 * PACKAGE:   qin
 * REVISION:  1.0
 * COPYRIGHT: Qin-domain
 * HISTORY:
 *     July, 23th, 2016  Rev1.0
 *             - Implement Memory_Barrier() and AtomicPoiter.
 * @endcode
 */
#include <stddef.h> //for NULL
#include <assert.h>
#include "qlib/qatomic.h"
#include "qlib/qmemory.h"

//Just suitable for __GNUC__ system, too lazy to make it better
#ifdef __GNUC__
#define MemoryBarrier()  __asm__ __volatile__("":::"memory")
#define CPURelax()       __asm__ __volatile__("pause\n":::"memory")
#else
#define MemoryBarrier()
#define CPURelax()
#endif

QAtomicPtr* QAtomicPtr_New()
{
    QAtomicPtr *aptr = (QAtomicPtr*)QMem_Malloc(sizeof(QAtomicPtr));
    assert(aptr != NULL);
    aptr->m_ptr = NULL;
    return aptr;
}

int QAtomicPtr_reset(QAtomicPtr *aptr)
{
    if(aptr == NULL)
        return QLIB_ERR_INVAL;

    MemoryBarrier();
	aptr->m_ptr = NULL;
    return QLIB_SUCCEEDED;
}

int QAtomicPtr_save(QAtomicPtr *aptr, void *value)
{
    if(aptr == NULL)
        return QLIB_ERR_INVAL;
    aptr->m_ptr = value;
    MemoryBarrier();
    return QLIB_SUCCEEDED;
}

void* QAtomicPtr_load(QAtomicPtr *aptr)
{
    void *result;

    if(aptr == NULL)
        return NULL;
    result = aptr->m_ptr;
    MemoryBarrier();
    return result;
}

