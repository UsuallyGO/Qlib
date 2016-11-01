
#include <string.h> //strlen
#include <assert.h> //assert
#include "qlib/qgeneral.h"
#include "qlib/qmemory.h"

//Rewrite this file
//more string code. 
//string expand, length of 2 powers
//string insert, safety memory move

char* QStrdup(const char *str)
{
    char* newstr;

    newstr = QMem_Malloc(strlen(str)+1);
    assert(newstr);
    strcpy(newstr, str);
    newstr[strlen(str)] = '\0';
    return newstr;
}

static int* _GetNext(const char *ptn)
{
    int  len, index, preIn;
    int* next;

    len = strlen(ptn);
    next = (int*)QMem_Malloc(len*sizeof(int));
    assert(next);
    memset(next, 0, sizeof(int)*len);
    preIn = 0;

    //next[0] must be 0
    for(index = 1; index < len; index++)
    {
        while(preIn > 0 && ptn[preIn] != ptn[index])
            preIn = next[preIn - 1];

        if(ptn[index] == ptn[preIn])
            preIn++;
        next[index] = preIn;
    }

    return next;
}

int QStrkmp(const char *str, const char *ptn)
{
    int slen, plen;
    int si, sign, preIn;
    int *next;

    slen = strlen(str);
    plen = strlen(ptn);
    if(plen > slen)
        return QLIB_ERR_DONE;
    if(plen == 0)
        return QLIB_SUCCEEDED;

    next = _GetNext(ptn);
    si = 0, sign = 0, preIn = 0;

    for(si = 0; si < slen; si++)
    {
        while(preIn > 0 && str[si] != ptn[preIn])
            preIn = next[preIn - 1];

        if(str[si] == ptn[preIn])
            preIn++;
        if(preIn == plen)
        {
            sign = 1;
            break;
        }
    }

    QMem_Free(next);
    if(sign)
        return si - plen + 1;
    return QLIB_ERR_DONE;
}

//Return the position of the longest prefix of ptn in str
int QStrprefix(const char *str, const char *ptn, int *length)
{
    int slen, plen;
    int si, preIn, sign;
    int mp, max;
    int *next;

    slen = strlen(str);//str may be shorter than ptn
    plen = strlen(ptn);
    *length = 0;

    if(plen == 0)
        return QLIB_SUCCEEDED;//0

    if(slen == 0)
        return QLIB_ERR_DONE;

    si = 0, preIn = 0;
    sign = 0, max = 0, mp = 0;
    next = _GetNext(ptn);
    for(si = 0; si < slen; si++)
    {
        while(preIn > 0 && str[si] != ptn[preIn])
            preIn = next[preIn - 1];

        if(str[si] == ptn[preIn])
        {
            preIn++;
            if(max < preIn)
            {
                max = preIn;
                mp = si;
            }
            sign = 1;
        }
        if(preIn == plen)
            break;
    }

    QMem_Free(next);
    *length = max;
    max = mp - max + 1;
    if(sign == 0)
        max = QLIB_ERR_DONE;

    return max;
}