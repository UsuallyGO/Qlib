//
/* For more reading materials, please read:
 * http://www.partow.net/programming/hashfunctions/index.html
 *
 * The implementation of hash tables is frequently called hashing. Hashing is a
 * technique used for performing insertions, deletions and finds in constant 
 * average time. Tree operations that require any ordering information among the
 * elements are not supported efficiently. Thus, operations such as find_min,
 * find_max and printing of the entire table in sorted order in linear time are
 * not supported.
 *
 * The main problem of hashing is that choosing a hashing function, deciding what
 * to do when two keys hash to the same value(known as collision), and deciding on
 * the table size.
 *
 * Generally, there are two kinds of input keys, integer or strings, and usually 
 * the keys are strings.
 *
 * If the input keys are integers, it is usually a good idea to ensure that the 
 * table size is prime. There several hash functions available for integer keys.
 * (1) Mod method. h(k) = k mod D, k is the key value, D is a certain number, 
 * usually it is related about the table size.
 * (2) Middle-square method. Caculate the square of the keys first and then get
 * the middle bits as the bucket address. In this method, if you take the r bits
 * numbers of the square, then the table size should the power of 2, such as 
 * 0 ~ (2^r - 1).
 * (3) Folding method. Divide the key into k parts with same length(except for the
 * last part). Then use the sum of the k parts to be hash address. Another similar
 * method is folding at the boundaries, this method transpose the odd parts then 
 * calculate the sum. For example, key = 12320324111220, in folding method, we set
 * k = 3 here, then P1 = 123, P2 = 203, P3 = 241, P4 = 112, P5 = 20. So,
 * h(key) = sum(Pi){i = 1, 2, 3, 4, 5}, h(key) = 123+203+241+112+20 = 699.
 * In folding at the boundaries method, h(key) = 123+302+241+211+20 = 897.
 *
 * For string keys, there are also more hash functions.(PJW and ELF is the worst, BKDR is 
 * the best? I don't know...)
 * (1) PJW Hash function
 *     This algorithm is based on work by Peter J.Weinberger of AT&T Bell Labs. The
 * book Compilers(Principles, Techniques and Tools, known as the dragon book)by Aho, 
 * Sethi and Ulman, recommends the use of hash functions that employ the hashing 
 * methodology found in this particula algorithm. Weinberger is also the as "W" of AWK
 * (the Unix patter match language).
 * (2) ELF Hash function
 *     Similar to PJW hash function, but tweaked for 32-bit processors(that means long 
 * type should be at least 32bits). Its the hash function widely used on most UNIX system.
 * (3) BKDR Hash function
 *     This hash function comes from Brian Kernighan and Dennis Ritchie's book "The C
 * Programming Language". It is a simple hash function using a strange set of possible 
 * seeds which all constitute a pattern of 31...31...31 etc., it seems to be vary similar
 * to the DJB hash function.
 * (4) AP Hash function
 *     An algorithm produced by Arash Partow, he took the ideas from all of the other hash
 * functions making a hybrid rotative and additive hash function algorithm. There isn't any 
 * real mathematical analysis explaining why one should use this hash functions instead of 
 * all the others. An empirical result which demonstrated the distributive abilities of the
 * hash algorithm was obtained using a hash-talbe with 100003 buckets, hashing The Project
 * Gutenberg Etex of Webster's Unabridged Dictionary. The longest encountered chain length 
 * was 7, the average chain length was 2, the number of empty buckets was 4579.
 * (5) DEK Hash function
 *    An algorithm proposed by Donald E.Knuth in "The Art Of Computer Programming Volume 3",
 * under the topic of sorting and search chapter 6.4.
 *
 *
 * How to deal with the "collision"? Commonly, there are several ways, this source has 
 * implemented the three ways below:
 * (1) Linear rehashing
 * (2) Nonlinear rehashing
 * (3) External chaining
 */
 
#ifndef _QLIB_HASH_H_
#define _QLIB_HASH_H_

#include "qgeneral.h"

//Hash table size more than 32768 can be user defined
#define HASH_MINIMAL_SIZE 9  //(2^9), 512
#define HASH_MIDDLE_SIZE  11 //(2^11), 2048 
#define HASH_LARGE_SIZE   13 //(2^13), 8192
#define HASH_MAXIMAL_SIZE 15 //(2^15), 32768

#define HASH_PJW_FUNC  0
#define HASH_ELF_FUNC  1
#define HASH_BKDR_FUNC 2
#define HASH_AP_FUNC   3
#define HASH_DEK_FUNC  4

#define HASH_LINEAR_REHASHING     0
#define HASH_NONLINEAR_REHASHING  1
#define HASH_EXTERNAL_CHAIN       2

typedef struct Hash_Table_T   QHashTable;
typedef struct Hash_Element_T QHashElement;
typedef union  Hash_Value_T   QHashValue;
typedef void*  UDefType;
//typedef unsigned long long ULL;
//typedef unsigned long UL;

typedef unsigned int (*hashFunc)(const char *datum);
typedef DataDestroyFunc valueDesFunc;

union Hash_Value_T{
    int      m_int;
    char*    m_str;
    double   m_double;
    UDefType m_udefine; //user define data type, pointer
    UL       m_u32;
    ULL      m_u64;
};

struct Hash_Element_T{
    QHashElement *m_next;
    char         *m_key;
    QHashValue    m_value;
};

struct Hash_Table_T{
    int           m_dataType;
    int           m_colDeal;
    unsigned int  m_capacity;
    unsigned int  m_counter;
    hashFunc      m_hashFunc;
    valueDesFunc  m_vdesFunc;
    QHashElement *m_value[1]; //user data stores in QHashElement
};                            //why is *m_value not m_value? Save more memory

#define QHashTable_set(T, K, V) QHashTable_seT((T),(K),(ULL)(V))

int  QHashTable_create(QHashTable *table, int dataType, int hashFunc, 
                      int colDeal, valueDesFunc vdesFunc, unsigned int size);
int  QHashTable_seT(QHashTable *table, const char *key, ULL value);
int  QHashTable_get(QHashTable *table, const char *key, void *value);
int  QHashTable_delete(QHashTable *table, const char *key);
void QHashTable_clear(QHashTable *table);
void QHashTable_destroy(QHashTable *table);

#endif //!_QLIB_HASH_H_