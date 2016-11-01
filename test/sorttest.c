
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qlib/qsort.h"

typedef struct Fraction_T Fraction;

struct Fraction_T
{
    int m_numerator;
    int m_denominator;
};

int Cmp(const void *data1, const void *data2)
{
    int frac1_numer, frac2_numer, diff;

    frac1_numer = ((Fraction*)data1)->m_numerator * ((Fraction*)data2)->m_denominator;
    frac2_numer = ((Fraction*)data2)->m_numerator * ((Fraction*)data1)->m_denominator;

    diff = frac1_numer - frac2_numer;
    if(diff > 0)
        return 1;
    else if(diff == 0)
        return 0;
    else
        return -1;
}

void printFrac(Fraction *array, int length)
{
    int index;

    for(index = 0; index < length; index++)
        printf("%2d/%-2d  ", array[index].m_numerator, array[index].m_denominator);

    printf("\n");
}

/*
int main()
{
    int index, tmp;
    Fraction frac[10];
    Fraction frac_b[10], frac_i[10], frac_q[10];
    Fraction frac_s[10], frac_m[10], frac_h[10];
    QSort sort;

    srand(time(0));
    for(index = 0; index < 10; index++)
    {
        frac[index].m_numerator = rand()%20;
        frac[index].m_denominator = rand()%20 + 1;
    }

    memcpy(frac_b, frac, sizeof(frac));
    memcpy(frac_i, frac, sizeof(frac));
    memcpy(frac_q, frac, sizeof(frac));
    memcpy(frac_s, frac, sizeof(frac));
    memcpy(frac_m, frac, sizeof(frac));
    memcpy(frac_h, frac, sizeof(frac));

    printf("Before sort:\n");
    printFrac(frac, 10);

    QSort_config(&sort, QSORT_ASCEND, QSORT_SORT_SELECT);//this is the default way
    QSort_set(&sort, frac, 0, 10, sizeof(Fraction), Cmp);
    QSort_sort(&sort);
    printf("After select sort Ascend :\n");
    printFrac(frac, 10);

    QSort_config(&sort, QSORT_ASCEND, QSORT_SORT_BUBBLE);
    QSort_set(&sort, frac_b, 0, 10, sizeof(Fraction), Cmp);
    QSort_sort(&sort);
    printf("After bubble sort Ascend :\n");
    printFrac(frac_b, 10);

    QSort_config(&sort, QSORT_DESCEND, QSORT_SORT_INSERT);
    QSort_set(&sort, frac_i, 0, 10, sizeof(Fraction), Cmp);
    QSort_sort(&sort);
    printf("After insert sort Descend :\n");
    printFrac(frac_i, 10);

    QSort_config(&sort, QSORT_DESCEND, QSORT_SORT_QUICK);
    QSort_set(&sort, frac_q, 0, 10, sizeof(Fraction), Cmp);
    QSort_sort(&sort);
    printf("After quick sort Descend :\n");
    printFrac(frac_q, 10);

    QSort_config(&sort, QSORT_ASCEND, QSORT_SORT_SHELL);
    QSort_set(&sort, frac_s, 0, 10, sizeof(Fraction), Cmp);
    QSort_sort(&sort);
    printf("After shell sort Ascend :\n");
    printFrac(frac_s, 10);

    QSort_config(&sort, QSORT_DESCEND, QSORT_SORT_MERGE);
    QSort_set(&sort, frac_m, 0, 10, sizeof(Fraction), Cmp);
    QSort_sort(&sort);
    printf("After merge sort descend :\n");
    printFrac(frac_m, 10);

    QSort_config(&sort, QSORT_DESCEND, QSORT_SORT_HEAP);
    QSort_set(&sort, frac_h, 0, 10, sizeof(Fraction), Cmp);
    QSort_sort(&sort);
    printf("After heap sort descend :\n");
    printFrac(frac_h, 10);

    QMemDumpInfo(0);
    return 0;
}
*/

int Cmp2(const void *p1, const void *p2)
{
    int c1, c2;

    c1 = *(int*)p1;
    c2 = *(int*)p2;

    if(c1 > c2)
        return 1;
    else if(c1 == c2)
        return 0;
    else
        return -1;
}

int main()
{
    int index;
    int nums[20];
    QSort sort;

    printf("Before sort:\n");
    for(index = 0; index < 20; index++)
    {
        nums[index] = 25 - index;
        printf("%d ", nums[index]);
    }
    printf("\n");

    QSort_config(&sort, QSORT_DESCEND, QSORT_SORT_QUICK);
    QSort_set(&sort, nums, 0, 20, sizeof(int), Cmp2);
    QSort_sort(&sort);

    for(index = 0; index <20; index++)
        printf("%d ", nums[index]);
    printf("\n");
    return 0;
}
