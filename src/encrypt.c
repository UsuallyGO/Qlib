
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "qlib/qgeneral.h"

#define DES_ENCRYPT 0
#define DES_DECRYPT 1

static int keyTransTable[] = 
{
  57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
  10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
  63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
  14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4,
};

static int keyPermuteTable[] = 
{
  14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
  23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
  41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
  44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32,
};

static int keyShiftTable[] = 
{
  1, 1, 2, 2, 2, 2, 2, 2, 
  1, 2, 2, 2, 2, 2, 2, 1,
};

static int dataPermuteTable[] = 
{
  58, 50, 42, 34, 26, 18, 10,  2,
  60, 52, 44, 36, 28, 20, 12,  4,
  62, 54, 46, 38, 30, 22, 14,  6,
  64, 56, 48, 40, 32, 24, 16,  8,
  57, 49, 41, 33, 25, 17,  9,  1,
  59, 51, 43, 35, 27, 19, 11,  3,
  61, 53, 45, 37, 29, 21, 13,  5,
  63, 55, 47, 39, 31, 23, 15,  7,
};

static int dataExpandTable[] = 
{
  32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
   8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
  16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
  24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1,
};

static int dataSubTable[][4][16] = 
{
  {
    {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
    { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
    { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
    {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},  
  },
  {
    {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
    { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
    { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
    {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
  },
  {
    {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
    {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
    {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
    { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
  },
  {
    { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
	{13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
	{10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
	{ 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
  },
  {
    { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
    {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
    { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
    {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
  },
  {
    {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
    {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
    { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},	 
    { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},

  },
  {
    { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
    {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
    { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
    { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
  },
  {
    {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},	
    { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},	
    { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
    { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11},
  },
};

static int dataScatterTable[] = 
{
  16,  7, 20, 21, 29, 12, 28, 17,
   1, 15, 23, 26,  5, 18, 31, 10,
   2,  8, 24, 14, 32, 27,  3,  9, 
  19, 13, 30,  6, 22, 11,  4, 25,
};

static int dataFinalTable[] = 
{
  40,  8, 48, 16, 56, 24, 64, 32, 
  39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30, 
  37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28, 
  35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26, 
  33,  1, 41,  9, 49, 17, 57, 25,
};

static unsigned char subkeys[16][7];

void bit_set(unsigned char *bits, int pos, int state)
{
    unsigned char mask;
    int index;

    mask = 0x80;
    for(index = 0; index < (pos % 8); index++)
        mask >>= 1;

    if(state)//set 1
        bits[pos/8] = bits[pos/8] | mask;
    else//set 0
        bits[pos/8] = bits[pos/8] & (~mask);
}

int bit_get(const unsigned char *bits, int pos)
{
    unsigned char mask;
    int index;

    mask = 0x80;
    for(index = 0; index < (pos % 8); index++)
        mask >>= 1;

    return (((mask & bits[pos/8]) == mask) ? 1 : 0);
}

void bit_rot_left(unsigned char *bits, int size, int count)
{
    int lbit, sbit;
    int index, bindex;

    if(size <= 0)
        return;

    for(index = 0; index < count; index++)
    {
        for(bindex = 0; bindex <= ((size - 1)/8); bindex++)
        {
            lbit = bit_get(&bits[bindex], 0);
            if(bindex == 0)
                sbit = lbit;
            else
                bit_set(&bits[bindex-1], 7, lbit);

            bits[bindex] <<= 1;
        }

        bit_set(bits, size-1, sbit);
    }
}

void bit_xor(const unsigned char *bits1, const unsigned char *bits2, 
             unsigned char *bitsx, int size)
{
    int index;

    for(index = 0; index < size; index++)
    {
        if(bit_get(bits1, index) != bit_get(bits2, index))
            bit_set(bitsx, index, 1);
        else
            bit_set(bitsx, index, 0);
    }
}

static void permute(unsigned char *bits, const int *mapping, int n)
{
    unsigned char temp[8];
    int index;

    memset(temp, 0, (int)ceil(n/8));
    for(index = 0; index < n; index++)
        bit_set(temp, index, bit_get(bits, mapping[index] - 1));

    memcpy(bits, temp, (int)ceil(n/8));
}

static unsigned char subkeys[16][7];

int QDes_keygen(const unsigned char *key)
{
    unsigned char dupbuf[8];
    unsigned char lkey[4], rkey[4];
    int index, j;

    if(key == NULL)
        return QLIB_ERR_INVAL;

    memcpy(dupbuf, key, 8);
    permute(dupbuf, keyTransTable, 56);
    memset(lkey, 0, 4);
    memset(rkey, 0, 4);

    for(index = 0; index < 28; index++)
        bit_set(lkey, index, bit_get(dupbuf, index));
    for(index = 0; index < 28; index++)
        bit_set(rkey, index, bit_get(dupbuf, index+28));

    memset(subkeys, 0, sizeof(subkeys));
    for(index = 0; index < 16; index++)
    {
        bit_rot_left(lkey, 28, keyShiftTable[index]);
        bit_rot_left(rkey, 28, keyShiftTable[index]);

        for(j = 0; j < 28; j++)
            bit_set(subkeys[index], j, bit_get(lkey, j));
        for(j = 0; j < 28; j++)
            bit_set(subkeys[index], j+28, bit_get(rkey, j));

        permute(subkeys[index], keyPermuteTable, 48);
    }

    return QLIB_SUCCEEDED;
}


static _des(const unsigned char *source, unsigned char *target, int type)
{
    unsigned char dupbuf[8];
    unsigned char lblk[6], rblk[6];
    unsigned char fblk[6], xblk[6];
    unsigned char sblk;
    int index, j, row, col;
    int count, k;

    if(source == NULL || target == NULL)
        return QLIB_ERR_INVAL;

    //Shoud use QDes_keygen() generate subkey before
    memcpy(dupbuf, source, 8);
    permute(dupbuf, dataPermuteTable, 64);
    memcpy(lblk, &dupbuf[0], 4);
    memcpy(rblk, &dupbuf[4], 4);

    memset(xblk, 0, 6);
    for(index = 0; index < 16; index++)
    {
        memcpy(fblk, rblk, 4);
        permute(fblk, dataExpandTable, 48);//expand from 32bits to 48bits

        if(type == DES_ENCRYPT)
        {
            bit_xor(fblk, subkeys[index], xblk, 48);
            memcpy(fblk, xblk, 6);
        }
        else
        {
            bit_xor(fblk, subkeys[15-index], xblk, 48);
            memcpy(fblk, xblk, 6);
        }

        count = 0;
        for(j = 0; j < 8; j++)
        {
            row = bit_get(fblk, (j*6)+0)*2 + bit_get(fblk, (j*6)+5)*1;
            col = bit_get(fblk, (j*6)+1)*8 + bit_get(fblk, (j*6)+2)*4 +
                  bit_get(fblk, (j*6)+3)*2 + bit_get(fblk, (j*6)+4)*1;

            sblk = (unsigned char)dataSubTable[j][row][col];
            for(k = 4; k < 8; k++)
            {
                bit_set(fblk, count, bit_get(&sblk, k));
                count++;
            }
        }

        permute(fblk, dataScatterTable, 32);
        bit_xor(lblk, fblk, xblk, 32);
        memcpy(lblk, rblk, 4);
        memcpy(rblk, xblk, 4);
    }

    memcpy(&target[0], rblk, 4);
    memcpy(&target[4], lblk, 4);

    permute(target, dataFinalTable, 64);
}

int QDes_encrypt(const unsigned char *text, unsigned char *entext)
{
    if(text == NULL || entext == NULL)
        return QLIB_ERR_INVAL;

    _des(text, entext, DES_ENCRYPT);
    return QLIB_SUCCEEDED;
}

int QDes_decrypt(const unsigned char *entext, unsigned char *detext)
{
    if(entext == NULL || detext == NULL)
        return QLIB_ERR_INVAL;

    _des(entext, detext, DES_DECRYPT);
    return QLIB_SUCCEEDED;
}
