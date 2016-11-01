
#include <assert.h>
#include "qlib/qvector.h"

int main()
{
    int res;
    QVector* vector;

    vector = (QVector*)QMem_Malloc(sizeof(QVector));
    QVector_Initial(vector, VALUE_TYPE_ADT, VECTOR_DEFAULT_SIZE);
    assert(res == VECTOR_DEFAULT_SIZE);//return value is the size of real allocated

    QMemDumpInfo(0);
    return 0;
}
