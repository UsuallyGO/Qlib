
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "qlib/qmemory.h"
#include "qlib/qlist.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

typedef struct UserData_T userData;

struct UserData_T{
    int    m_id;
    char  *m_name;
    double m_salary;
};

int dataCmp(const void *data1, const void *data2)
{
    return strcmp(((userData*)data1)->m_name, ((userData*)data2)->m_name);
}

void dataDestroy(void *data)
{
    userData *ptr;
    ptr = (userData*)data;
    if(ptr != NULL){
        if(ptr->m_name != NULL)
            QMem_Free(ptr->m_name);
        QMem_Free(ptr);
    }
}

void ListTest()
{
    QList *list;
    userData *data;
    int index = 0;
    QListIterator iter;
    char nameArr[10][50] = { "Micheal Jordan", "Maxwell Qin",
        "Andrew Peterson", "John Monkey", "Cristina Ronaldo", 
        "Tesla Motor", "Lucas Boy", "Nichalos Xie",
        "Lucky Qin", "Tiage Xierwa" };
    double salaryArr[10] = {1000000, 200000, 1500000, 250000, 
           2230000, 4560000, 50000, 983333, 1200000, 4000000};
    

    list = (QList*)QMem_Malloc(sizeof(QList));
    assert(list);
    QList_init(list, dataCmp, dataDestroy);

    for(index = 0; index < 10; index++){
        data = (userData*)QMem_Malloc(sizeof(userData));
        assert(data);
        data->m_id = index;
        data->m_name = (char*)QMem_Malloc(50);
        snprintf(data->m_name, 50, "%s", nameArr[index]);
        data->m_salary = salaryArr[index];
        if(index%2 == 0)
            QList_addTail(list, data);
        else
            QList_addHead(list, data);
    }
    
    index = QList_getCount(list);
    printf("List has %d nodes.\n", index);
    QListIterReset(&iter, list);
    data = NULL;
    while((data=(userData*)QListIterNext(&iter)) != NULL){
        printf("ID:    %d\n", data->m_id);
        printf("Name:  %s\n", data->m_name);
        printf("Salary:%lf\n\n", data->m_salary);
    }
    QList_clearAll(list);
    index = QList_getCount(list);
    printf("List has %d nodes.\n", index);
    QMem_Free(list);
}

int main()
{
    ListTest();

    QMemDumpInfo(0);
    return 0;
}
