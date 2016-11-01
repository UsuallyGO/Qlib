
#ifndef _QLIB_STRING_H_
#define _QLIB_STRING_H_

char* QStrdup(const char* str);
int QStrkmp(const char *str, const char *pattern);
int QStrprefix(const char *str, const char *ptn, int *length);

#endif //!_QLIB_STRING_H_
