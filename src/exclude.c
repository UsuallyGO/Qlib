
#include <stdio.h>
#include <assert.h> //assert
#include <errno.h> //EBUSY 
#include "qlib/qmemory.h"
#include "qlib/qgeneral.h"
#include "qlib/qexclude.h"

#ifdef _MSC_VER
#include <windows.h> //for InterlockedExchange
#include <process.h> //for YieldProcessor()
#include <intrin.h> //for _ReadWriteBarrier()
#endif

#ifdef __linux__
#include <pthread.h> //for mutex with pthread lib
#elif defined _WIN32
#include <windows.h>
#endif

void QSpin_init(QSpinlock *lock)
{
    *lock = QSPIN_LOCK_OFF;
}

/* Atomic exchange of various sizes
 *
 * The xchg_xx family functions not exchange oper1 and oper2, they are just like this:
 * xchg_xx(a, b):
 *   tmp = a;
 *   a = b;
 *   return a;
 */
#ifdef __GNUC__
#define barrier()   __asm__ __volatile__("":::"memory")
#define cpu_relax() __asm__ __volatile__("pause\n":::"memory")

static inline ULL xchg_64(void *ptr, ULL x)
{
    __asm__ __volatile__("xchgq %0, %1"
                         :"=r" ((ULL)x)
                         :"m"  (*(volatile ULL*)ptr), "0"((ULL)x)
                         :"memory");
    return x;
}

static inline UI xchg_32(void *ptr, UI x)
{
    __asm__ __volatile__("xchgl %0, %1"
                         :"=r" ((UI)x)
                         :"m" (*(volatile UL*)ptr), "0"((UI)x)
                         :"memory");
    return x;
}

static inline US xchg_16(void *ptr, unsigned short x)
{
    __asm__ __volatile__("xchgw %0, %1"
                         :"=r" ((US)x)
                         :"m" (*(volatile US*)ptr), "0"((US)x)
                         :"memory");
    return x;
}

/* Test and set a bit
 * This function is just like this:
 * if( ptr && (2<<x))
 *    return -1;
 * else
 *    set ptr's Xth bit to be one;
 *    return 0;
 */
static inline char atomic_bit_setOne(void *ptr, int x)
{
    char value;
    __asm__ __volatile__("lock; bts %2, %1\n"
                         "sbb %0, %0\n"
                         :"=r"(value), "=m"(*(volatile UL*)ptr)
                         :"Ir" (x)
                         :"memory");
   return value;
}

void QSpin_lock(QSpinlock *lock)
{
    while(1)
    {
        if(!xchg_32(lock, QSPIN_LOCK_ON))
            return;
        while(*lock)
            cpu_relax();
    }
}

void QSpin_unlock(QSpinlock *lock)
{
    barrier();
    *lock = QSPIN_LOCK_OFF;
}

//not spin
int QSpin_tryLock(QSpinlock *lock)
{
    return !xchg_32(lock, QSPIN_LOCK_ON);
}

#elif defined _MSC_VER
#pragma intrinsic(_ReadWriteBarrier)

void QSpin_lock(QSpinlock *lock)
{
    while(1)
    {
        if(!InterlockedExchange(lock, QSPIN_LOCK_ON))
            return;
        while(*lock)
            YieldProcessor(); //this will help improve the performance of spin loops
    }
}
void QSpin_unlock(QSpinlock *lock)
{
    _ReadWriteBarrier();
    *lock = QSPIN_LOCK_OFF;
}
int  QSpin_tryLock(QSpinlock *lock)
{ 
    return !InterlockedExchange(lock, QSPIN_LOCK_ON);
}
#endif

#ifdef __linux__

//Mutex in linux should linked with pthread library(-lpthread)
//Do we need to implement without pthread???
QMutex* Mutex_new()
{
    int res;

    QMutex *mutex = (QMutex*)QMem_Malloc(sizeof(pthread_mutex_t));
    assert(mutex != NULL);
    res = pthread_mutex_init((pthread_mutex_t*)mutex, QMUTEX_ATTR_DEFAULT);
    if(res != QLIB_SUCCEEDED)
    {
        QMem_Free(mutex);
        mutex = NULL;
    }
    return mutex;
}

void QMutex_lock(QMutex *mutex)
{
    assert(mutex != NULL);
    //actually it has return value, 0 means locked successfully
    pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void QMutex_unlock(QMutex *mutex)
{
    assert(mutex != NULL);
    pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

int QMutex_trylock(QMutex *mutex)
{
    int res;

    assert(mutex != NULL);
    res = pthread_mutex_trylock((pthread_mutex_t*)mutex);
    switch(res)
    {
    case EBUSY:
        res = QLIB_ERR_DONE;//thread already locked before
        break;
    case 0:
        res = QLIB_SUCCEEDED; //lock successfully
        break;
    default:
        res = QLIB_ERR_SYSTEM;//lock failed
    }
    return !res;
}

#elif defined _WIN32

QMutex* Mutex_new()
{
    HANDLE  handle;
    HANDLE *mutex;

    handle = CreateMutex(NULL, FALSE, NULL);
    assert(handle != NULL);
    mutex = (HANDLE*)QMem_Malloc(sizeof(HANDLE));
    assert(mutex != NULL);
    *mutex = handle;
    return (QMutex*)mutex;
}

void QMutex_lock(QMutex *mutex)
{
    assert(mutex != NULL);
    /*waits until the specified object is in the signaled state
      or the time-out interval elapses. But the time-out interval
      here is INFINITIE.*/
    WaitForSingleObject(*(HANDLE*)mutex, INFINITE);
}

void QMutex_unlock(QMutex *mutex)
{
    assert(mutex != NULL);
    ReleaseMutex(*(HANDLE*)mutex);
}

int QMutex_trylock(QMutex *mutex)
{
    DWORD retVal;
    int res;

    assert(mutex != NULL);
    retVal = WaitForSingleObject(*(HANDLE*)mutex, 0);
    switch(retVal)
    {
    case WAIT_FAILED:
        res = QLIB_ERR_SYSTEM;
        break;
    case WAIT_TIMEOUT:
        res = QLIB_ERR_DONE;
        break;
    default:
        res = QLIB_SUCCEEDED;
    }
    return !res;
}
#endif

void QMutex_destroy(QMutex *mutex)
{
    if(mutex != NULL)
        QMem_Free(mutex);
}