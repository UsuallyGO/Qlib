
#ifndef _QLIB_MEMORY_H_
#define _QLIB_MEMORY_H_

#define QMem_Malloc(size)       QMem_malloc(size, __LINE__, __FILE__)
#define QMem_Realloc(ptr, size) QMem_realloc(ptr, size, __LINE__, __FILE__)
#define QMem_Free(ptr)          QMem_free(ptr)
#define QExc_Malloc(size)       QExc_malloc(size, __LINE__, __FILE__)
#define QExc_Free(ptr)          QExc_free(ptr, __LINE__, __FILE__)

void* QMem_malloc(unsigned int size, int line, const char* file);
void* QMem_realloc(void *ptr, unsigned int size, int line, const char* file);
void  QMem_free(void *ptr);
void* QExc_malloc(unsigned int size, int line, const char* file);
void  QExc_free(void *ptr, int line, const char* file);

void  QMemDumpInfo(int since);

#endif //!_QLIB_MEMORY_H_