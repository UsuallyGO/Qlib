
#include <stdio.h>
#include "qlib/qmemory.h"
#include "qlib/qexclude.h"

int main()
{
    QSpinlock t;
    QMutex *mutex;

    t = QSPIN_LOCK_OFF;

    if(QSpin_tryLock(&t))
        printf("locked\n");
    else
        printf("cannot lock\n");

    if(QSpin_tryLock(&t))
        printf("locked\n");
    else
        printf("cannot lock\n");

    QSpin_unlock(&t);
    if(QSpin_tryLock(&t))
        printf("locked\n");
    else
        printf("cannot lock\n");

    mutex = QMutex_new();
    QMutex_lock(mutex);
    printf("Mutex locked.\n");
    if(QMutex_trylock(mutex))
        printf("Mutex trylock.\n");
    else
        printf("Mutex trylock failed.\n");

    QMutex_unlock(mutex);
    if(QMutex_trylock(mutex))
        printf("Mutex trylock.\n");
    else
        printf("Mutex trylock failed.\n");	

    QMutex_destroy(mutex);
    QMemDumpInfo(0);
    return 0;
}
