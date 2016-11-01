
#ifndef _QLIB_VECTOR_H_
#define _QLIB_VECTOR_H_

#pragma pack(1)
struct Vector_t
{
    size_t m_size; //the memory size of m_data in bytes
    size_t m_capacity; //the element numbers of m_data
    size_t m_elemNums; //the numbers of element stored in m_data
    int    m_dataType; //the data type, int, double, string, ADT
    size_t m_elemSize; //the size of each element with type m_dataType
    unsigned char *m_data;  //stub of the data
};
#pragma pack()

struct VectorIter_t
{
};

#define VECTOR_MINIMAL_SIZE  128
#define VECTOR_SMALL_SIZE    256
#define VECTOR_MEDIUM_SIZE   512
#define VECTOR_LARGE_SIZE    1024
#define VECTOR_DEFAULT_SIZE  VECTOR_MINIMAL_SIZE

typedef struct Vector_t QVector;
int QVector_Initial(QVector *vector, int type, unsigned int size); //the size can be increased later

#endif//!_QLIB_VECTOR_H_
