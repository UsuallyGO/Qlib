
#include <stdio.h>
#include "qlib/qmemory.h"
#include "qlib/qstack.h"

//Just an easy testcase
void stacktest()
{
    QStack stack;
    QStackIterator iter;
    char *data;

    char *SAS = "San Antonio Spurs";
    char *MEM = "Memphis Grizzlies";
    char *HOU = "Houston Rockets";
    char *NOK = "New Orleans Hornets";
    char *MIN = "Minnesota Timberwolves";
    char *DEN = "Denver Nuggets";
    char *UTH = "Utah Jazz";
    char *POR = "Portland Trail Blazers";
    char *OCT = "Oklahoma City Thunder";


    QStack_init(&stack);
    QStack_push(&stack, SAS);
    QStack_push(&stack, MEM);
    QStack_push(&stack, HOU);
    QStack_push(&stack, NOK);

    QStackIterReset(&iter, &stack);
    while((data=(char*)QStackIterNext(&iter)) != NULL)
        printf("%s\n", data);

    QStack_pop(&stack);
    QStack_push(&stack, MIN);
    QStack_pop(&stack);
    QStack_push(&stack, DEN);
    QStack_push(&stack, UTH);
    QStack_pop(&stack);
    QStack_pop(&stack);
    QStack_push(&stack, POR);
    QStack_push(&stack, OCT);

    printf("==========================================\n");
    QStackIterReset(&iter, &stack);
    while((data=(char*)QStackIterNext(&iter)) != NULL)
        printf("%s\n", data);//Now: OCT POR HOU MEM SAS

    QStack_popAll(&stack);
}

int main()
{
    stacktest();
    QMemDumpInfo(0);

    return 0;
}
