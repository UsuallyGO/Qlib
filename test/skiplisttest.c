
#include <stdio.h>
#include <assert.h>
#include "qlib/qskiplist.h"

int cmp(const void *data1, const void *data2)
{
    int val1, val2;

    val1 = *(int*)data1;
    val2 = *(int*)data2;

    if(val1 < val2)
        return -1;
    else if(val1 == val2)
        return 0;
    else
        return 1;
}

void prt(void *data)
{
    int value;

    value = *(int*)data;
    printf("%d", value);
}

void swap(int *val1, int *val2)
{
    int tmp;

    tmp = *val1;
    *val1 = *val2;
    *val2 = tmp;
}

int main()
{
    QSkiplist *list;
    int array[40], index;
    int numbers[50];
    int length, tmp;

    list = QSkiplist_create(cmp, NULL);
    assert(list != NULL);

    for(index = 0; index < 50; index++)
        numbers[index] = index + 1;

    srand(time(0));
    printf("Before insert, the array:\n");
    length = 50;
    for(index = 0; index < 30; index++)
    {
        tmp = rand()%length;
        array[index] = numbers[tmp];
        swap(&numbers[tmp], &numbers[length-1]);
        length--;
        printf("%d ", array[index]);
        QSkiplist_insert(list, &array[index]);
    }

    QSkiplistPrint(list, prt);
    QSkiplist_destroy(list);
    QMemDumpInfo(0);
}
