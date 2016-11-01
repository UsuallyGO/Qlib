
#ifndef _QLIB_ATOMIC_H_
#define _QLIB_ATOMIC_H_

#include "qlib/qgeneral.h"

/*Pay attention please: this is only suitable for X86 architecture, more
  machines such ARM, MIPS etc. not support yet.
  Perhaps I am just too lazy to do this...
 */

typedef struct AtomicPointer_T QAtomicPtr;

struct AtomicPointer_T
{
    void* m_ptr; //void* can adapt for multi-systems
};

QAtomicPtr* QAtomicPtr_New();

int QAtomicPtr_reset(QAtomicPtr *aptr);

int QAtomicPtr_save(QAtomicPtr *aptr, void *value);

void* QAtomicPtr_load(QAtomicPtr *aptr);

#endif //!_QLIB_ATOMIC_H_
