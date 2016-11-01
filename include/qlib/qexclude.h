//
/* More material about this, please read: http://locklessinc.com/articles/locks/ or
 * <<Intel 64 and IA-32 Architectures Software Developer¡¯s Manual>> 
 * http://www.intel.com/content/www/us/en/architecture-and-technology/
 * 64-ia-32-architectures-software-developer-vol-3a-part-1-manual.html
 *
 * Perhaps this is a very SERIOUS problem: When do we need lock to implement synchronization???
 * In Qlib, all of these locks are in User space, so they should only work in multi-threads not multi-processes.
 * Is this right???
 *
 * A spinlock is a lock which causes a thread trying to acquire it to simply wait in a loop("spin") while repeatedly
 * checking if the lock is available. Since the thread remains active but is not  performing a useful task, the use 
 * of such a lock is a kind of "busy waiting". Once acquired, spinlocks will usually be held until they are explicitly
 * released, although in some implementations they may be auotmatically released if the thread being waited on(that 
 * which holds the lock) blocks, or "goes to sleep".
 * Because the avoid overhead from operating system process rescheduling or context switching, spinlocks are efficient 
 * if threads are likely to be blocked for only short periods. For this reason, operating-system kernels often use 
 * spinlocks.
 * However, spinlocks become wasteful if held for longer durations, as they may prevent other threads from running and 
 * require rescheduling. The longer a thread holds a lock, the greater the risk the thread will be interrupted by the OS
 * scheduler while holding the lock. If this happens, other threads will be left "spining"(repeatedly trying to acquire 
 * the lock), while the thread holding the lock is not making progress towards releasing it. The result is an indefinite
 * postponement until the thread holding the lock can finish and release it. This is especially true on a single-processor
 * system, where each waiting thread of the same priority is likely to waste its quantum(allocated time where a thread can
 * run) spinning until the thread that holds the lock is finally finished.
 *
 * Spinlock versus Semaphore
 * 1.What they are?
 *   A spin lock is one possible implementation of a lock, namely one that is implemented by busy waiting("spinning"). A 
 * semaphore is a generalization of a lock(or, the other way around, a lock is a special case of a semaphore). Usually, but
 * not necessarily, spinlocks are only valid within one process whereas semaphores can be used to synchronize between different
 * processes, too.
 *   A lock works for mutual exclusion, that is one thread at a time can acquire the lock and proceed with a "critical section"
 * of code. Usually, this means code that modifies some data shared by several threads.
 *   A semaphore has a counter and will allow itself being acquired by one or several threads, depending on what value you post
 * to it, and(in some implementations) depending on what its maximum allowable value is.
 *   Insofar, one can consider a lock a special case of a semaphore with a maximum value of 1.
 * 2.What they do?
 *   As stated above, a spinlock is a lock, and therefore a mutual exclusion(strictly 1 to 1) mechanism. It works by repeatedly
 * querying and/or modifying a memory location, usually in an atomic manner. This means that acquiring a spinlock is a "busy" 
 * operation that possibly burns CPU cycles for a long time(maybe forever!) while it effectively achieves "nothing".
 * The main incentive for such an approach is the fact that a context switch has an overhead equivalent to spinning a few hundred
 * (or maybe thousand)times, so if a lock can be acquired by burning a few cycles spinning, this may overall very well be more 
 * efficient. Also, for realtime applications it may not be acceptable to block and wait for the scheduler to come back to them 
 * at some far away time in the future.
 *   A semaphore, by contrast, either does not spin at all, or only spins for a very short time(as an optimization to avoid the
 * syscall overhead). If a semaphore cannot be acquired, it blocks, giving up CPU time to a different thread that is ready to 
 * run. This may of course mean that a few milliseconds pass before your thread is scheduled again, but if this is no problem
 * (usually it isn't) then it can be a very efficient, CPU-conservative approach.
 *
 *   When a thread tries to lock a spinlock and it does not succeed, it will continuously retry locking it, until it finally 
 * succeeds; thus it will not allow another thread to take its place(however, the operating system will forcefully switch to another
 * thread, once the CPU runtime quantum of the current thread has been exceeded, of course).
 *
 *   The problem with mutexes is that putting threads to sleep and waking them up again are both rather expensive operations, 
 * they'll need quite a lot of CPU instructions and thus also take some time. If now the mutex was only locked for a very short 
 * amount of time, the time spent in putting a thread to sleep and waking it up again might exceed the time the thread has actually
 * slept by far and it might even exceed the time the thread would have wasted by constantly polling on a spinlock. On the other hand, 
 * polling on a spinlock will constantly waste CPU time and if the lock is held for a longer amount of time, this will waste a lot 
 * more CPU time and it would have been much better if the thread was sleeping instead.
 *
 *Spinlock on single-core/multi-core CPU:
 *   Using spinlocks on a single-core/single-CPU system makes usually no sense, since as long as the spinlock polling is blocking the
 * only available CPU core, no other thread can run and since no other thread can run, the lock won't be unlocked either. IOW(In Other
 * Words), a spinlock wastes only CPU time on those systems for no real benefit. If the thread was put to sleep instead, another thread
 * could have ran at once, possibly unlocking the lock and then allowing the first thread to continue processing, once it work up again.
 *   On a multi-core/multi-CPU systems, with plenty of locks that are held for a very short amount of time only, the time wasted for 
 * constantly putting threads to sleep and waking them up again might decrease runtime performance noticeably. When using spinlocks instead,
 * threads get the chance to take advantage of their full runtime quantum(always only blocking for a very short time period, but then 
 * immediately continue their work), leading to much higher processing throughput.
 *
 * =============================================
 *
 * Atomic Operation
 *   An operation acting on shared memory is atomic if it completes in a single step relative to other threads.
 * When an atomic store is performed on a shared variable, no other thread can observe the modification half-complete
 * When an atomic load is performed on a shared variable, it reads the entire value as it appeared at a single
 * moment in time.
 *
 * Atomic Operation in CPU
 * - Intel CPU
 *   The Intel486 processor(and newer processors since) guarantees that the following basic memory operations will
 * always be carried out atomically:
 *   % Reading or writing a byte
 *   % Reading or writing a word aligned on a 16-bit boundary
 *   % Reading or writing a doubleword aligned on a 32-bit boundary
 *   The Pentium processor(and newer processors since) guarantees that the following additional memory operations
 * will always be carried out atomically:
 *   % Reading or writing a quadword aligned on a 64-bit boundary
 *   % 16-bit accesses to uncached memory locations that fit within a 32-bit data bus
 *   The P6 family processors(and newer processor since) guarantee that the following memory operation will always
 * be carried out atomically:
 *   % Unaligned 16-, 32-, and 64-bit accesses to cached memory that fit within a cache line
 *
 * NOTICE:
 *    The processor WORD size is a function of the architecture. Thus, a 32-processor has a 32-bit word. In software
 * types, including assembly, usually there is need to identify the size unambigously, so word type for historical 
 * reasons is 16-bits. So probably both sources are correct, if you read them in context: the first one is referring
 * to the processor word, while the Intel guide is referring to the word type.
 * - AMD CPU
 *    Atomicity of accesses. Single load or store operations(from instructions that do just a single load or store)
 * are naturally atomic on any AMD64 processor as long as they do not cross an aligned 8-byte
 *
 * Memory Ordering(Reording)
 * Reordering:
 *    Reads and writes do not always happen in the order that you have written them in your code.
 * Why reordering:
 *    Performance.
 * Reording Principle:
 *    In single threaded programs from the programmer's point of view, all operations appear to
 * have been executed in the order specified, with all inconsistencies hidden by hardware.
 *
 * Example1:
 * C programe:  int A, B;
 *              void foo(){
 *                  A = B + 1;
 *                  B = 0;
 *              }
 * Compile instruction: 
 *    gcc -S -masm=intel ordering.c  |  gcc -O2 -S -masm=intel ordering.c
 *      mov eax, DWORD PTR B[rip]    |    mov eax, DWORD PTR B[rip]
 *      add eax, 1                   |    mov DWORD PTR B[rip], 0
 *      mov DWORD PTR A[rip], eax    |    add eax, 1
 *      mov DWORD PTR B[rip], 0      |    mov DWORD PTR A[rip], eax
 *
 * Notice that, the assignment to A and B has been reordered in compile2.
 * Reordering-Type:
 *    Compiler Reordering, the reordering happened in compiling.
 *    Memory Reordering, the reordering happened in excuting.
 *
 * The running efficiency can be benefited from reordering, especially for parallel programming.
 * But sometimes, we don't need reordering, there should be a way for users to shut down the
 * compiler's reordering. This is the so called Compiler Memory Barrier.
 *
 * Memory Barrier:
 *    A memory barrier, is a type of barrier instruction which causes a central processing unit 
 *(CPU) or compiler to enforce an ordering constraint on memory operations issused before and 
 * after the barrier instruction. This typically means that certain operations are guaranteed to
 * performed before the barrier, and others after.
 *
 * Compiler Memory Barrier Instructions:
 * --GNU
 *   asm volatile("":::"memory");
 *   __asm__ __volatile__("":::"memory");
 * --Intel ECC Compiler
 *   __memory_barrier();
 * --Microsoft Visual C++
 *   _ReadWriteBarrier();
 *
 * The compile result of the same program with compiler memory barrier.
 * C language program:
 * int A, B;
 * void foo(){
 *    A = B + 1;
 *    __asm__ __volatile__("":::"memory");
 *    B = 0;
 * }
 * Compile instruction and its result:
 * gcc -O2 -S -masm=intel ordering.c
 * mov eax, DWORD PTR B[rip]
 * add eax, 1
 * mov DWORD PTR A[rip], eax
 * mov DWORD PTR B[rip], 0
 *
 * Compiler Memroy Barrier instructions can be only effected with its context instructions.
 *
 * CPU memory barrier:
 * The term memory ordering refers to the order in which the processor issues reads(loads) and wirtes(stores)
 * throught the system bus to system memory.
 * CPU Reordering Types:
 * 2 instructions, 2 operation types: read(load) and write(store)
 * 4 reorderig types: Loadload, loadstore, storeload, storestore
 *
 * CPU Memory Models:
 *    Memory consistency models describe how threads may interact through shared memory consistency.
 * There are many types of memory reordering, and not all types of reordering occur equally often. It
 * all depends on processor you're targeting and/or the tool chain you're using for development.
 *
 * Intel X86/64 Memory Model
 * In a single-processor system for memory regions defined as write-back cacheable.
 *    - Reads are not reordered with other reads.
 *    - Writes are not reordered with older reads.
 *    - Writes to memory are not reordered with other writes.
 *    - Reads may be reordered with older writes to different locations but not with older writes to 
 *      the same location.
 * In a multiple-processor system
 *    - Individual processors use the same ordering principles as in a single-processor system.
 *    - Writes by a single processor are observed in the same order by all processors.
 *    - Writes from an individual processor are NOT ordered with respect to the writes from other
 *      processors.
 *    - Memory ordering obeys causality(memory ordering respects transitive visibility).
 *    - Any two stores are seen in a consistent order by processors other than those perfoming the 
 *      stores.
 *
 * Memory Barrier Instructions in CPU
 * x86, x86-64, amd64
 * - lfence:  Load Barrier
 * - sfence:  Store Barrier
 * - mfence:  Full Barrier
 * PowerPc, MIPS
 * - sync:    Full Barrier
 * Itanium
 * - mf:      Full Barrier
 * ARMv7
 * - dmb: Data Memory Barrier
 * - dsb: Data Synchronization Barrier
 * - isb: Instruction Synchronization Barrier
 *
 * Use CPU Memory Barrier Instructions(x86)
 * Only CPU Memory Barrier
 * - asm volatile("mfence");
 * CPU + Compiler Memory Barrier
 * - asm volatile("mfence":::"memory");
 *
 * Use Lock instruction to implement a MB
 * lock addl
 * - asm volatile("lock; addl $0,0(%%esp)":::"memory")
 *   addl $0, 0(%%esp)   -> nothing serious
 *   lock  -> to be a cpu memory barrier
 *   "memory"  -> to be a compiler memory barrier
 *
 * xchg
 * - asm volatile("xchgl(%0), %0":::"memory")
 *   why xchg don't need lock prefix? The LOCK prefix is automatically assumed for XCHG instruction.
 *
 * Some implementations in Linux
 * #ifdef CONFIG_X86_32
 * #define mb()  alternative("lock:addl $0, 0(%%esp)", "mfence", X86_FEATURE_XMM2)
 * #defien rmb() alternative("lock:addl $0, 0(%%esp)", "lfence", X86_FEATURE_XMM2)
 * #define wmb() alternative("lock:addl $0, 0(%%esp)", "sfence", X86_FEATURE_XMM)
 * #else
 * #define mb()  asm volatile("mfence":::"memory")
 * #define rmb() asm volatile("lfence":::"memory")
 * #define wmb() asm volatile("sfence":::"memory")
 *
 * MemoryBarrier in Windows(x86, x86-64)
 * FORCEINLINE VOID MemoryBarrier(VOID){
 *    LONG Barrier;
 *    __asm{ xchg Barrier,eax}
 * }
 */

#ifndef _QLIB_EXCLUDE_H_
#define _QLIB_EXLCUDE_H_

#define QSPIN_LOCK_ON     1
#define QSPIN_LOCK_OFF    0

#ifdef __GNUC__
typedef unsigned int QSpinlock;
#elif defined _MSC_VER
typedef long QSpinlock;
#endif

#ifdef __GNUC__
#define QMUTEX_ATTR_DEFAULT  PTHREAD_MUTEX_DEFAULT
#endif

//!!!Pay attention: QMutex CAN be acquired twice by the same thread in windows

typedef struct _Mutex QMutex;//opaque datastruct, puzzled?

#define QMutex_new() Mutex_new();

QMutex* Mutex_new();
void QMutex_lock(QMutex *mutex);
void QMutex_unlock(QMutex *mutex);
int  QMutex_trylock(QMutex *mutex);
void QMutex_destroy(QMutex *mutex);

void QSpin_init(QSpinlock *lock);
void QSpin_lock(QSpinlock *lock);
void QSpin_unlock(QSpinlock *lock);
int  QSpin_tryLock(QSpinlock *lock);

#endif //!_QLIB_EXCLUDE_H_
