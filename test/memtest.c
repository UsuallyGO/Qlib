
#include <stdio.h>
#include <assert.h>
#include "qlib/qexception.h"
#include "qlib/qmemory.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

void TestmemExc()
{
    char *str;
    int *intp;

	printf("\nIn TestmemExc().\n");

    try {
        str = (char*)QExc_Malloc(100);
    }
    catch_excp(Mem_AllocFailed){
        printf("Mem_AllocFailed exception has been caught.\n");
    }
    try_end;

    snprintf(str, 100, "Memory allocated by QMem_Malloc().\n");
    try{
        QExc_Free(str);
        QExc_Free(str); //Free twice, there must be a Mem_FreeFailed exception
    }
    catch_excp(Mem_FreeFailed){
        printf("Mem_FreeFailed exception has been caught.\n");
    }
    try_end;

    try{
        QExc_Free(str);//Mem_FreeFailed exception thrown here
    }
    catch_excp(Mem_AllocFailed){
        printf("Mem_AllocFailed, should not be caught here.\n");
    }
    catch_excp(Exc_General){
        printf("Unknow exception, should not be caught here.\n");
    }
    uncaught_excp{
        printf("Uncaught exception, should be here.\n");
    }
    try_end;
}

void Testmem()
{
    char *str;
    int  *intp;
    char *cp;

    printf("\nIn Testmem().\n");

    str = (char*)QMem_Malloc(50);
    assert(str); //should be not null here...
    snprintf(str, 50, "Memory allocate by QMem_Malloc().\n");
    intp = (int*)QMem_Malloc(sizeof(int));
    assert(intp);
    *intp = 10;
    cp = (char*)QMem_Malloc(sizeof(char));
    assert(cp);
    *cp = 'x';

    printf("str:%s", str);
	printf("intp:%d cp:%c\n\n", *intp, *cp);
    QMem_Free(str);
    QMem_Free(intp);
    QMem_Free(cp);
}

int main()
{
    char *strExc;
    char *str;

    TestmemExc();
    Testmem();

    try{
        str = (char*)QExc_Malloc(50);
    }
	try_end;

    str = (char*)QMem_Malloc(50);
    assert(str);

    QMemDumpInfo(0);
    return 0;
}
