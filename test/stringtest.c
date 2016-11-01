#include <stdio.h>
#include "qlib/qgeneral.h"
#include "qlib/qstring.h"

void kmptest()
{
    int res;
    char str[] = "abcdefaaacbddcdefgaabd";
    char ptn[] = "defga";

    res = 0;
    res = QStrkmp(str, ptn);
    if(res == QLIB_ERR_DONE)
        printf("KMP Not matched.\n");
    else
        printf("KMP Matched from: %d\n", res);
}

void prefixtest()
{
    int res, length;
    char str[] = "";
    char ptn[] = "ggggggg";

    res = QStrprefix(str, ptn, &length);
    printf("Prefix res:%d length:%d\n", res, length);
}

int main()
{
    kmptest();
    prefixtest();

    QMemDumpInfo(0);
    return 0;
}

