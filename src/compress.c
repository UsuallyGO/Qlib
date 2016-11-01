
#include <stdio.h> // FILE
#include <assert.h> //assert
#include <string.h> //memeset
#include "qlib/qmemory.h"
#include "qlib/qgeneral.h"

#define SENTINEL     0x7F
#define SAME_CHAR    0
#define DIFF_CHAR    1
#define BUFFER_SIZE  512
#define ALGO_HUFFMAN 0x00
#define ALGO_RLE     0x01

//What an UGLY code!!!
int QCompress_encodeRLE(FILE *fileIn, FILE *fileOut)
{
    char  prev;
    UCHAR counter;
    UINT  bytes, length;
    int   index, changed;
    char  buf[BUFFER_SIZE];

    bytes = 0;
    counter = 0;
    changed = SAME_CHAR;
    while(1)
    {
        length = fread(buf, sizeof(char), BUFFER_SIZE, fileIn);
        if(length == 0)//no more file
        {
            if(counter == 1)
                fprintf(fileOut, "%c", prev);
            else if(counter != 0)
                fprintf(fileOut, "%c%c%c", SENTINEL, prev, counter);

            break;
        }
        if(bytes == 0)
            prev = buf[0];

        for(index = 0; index < (int)length; index++)
        {
            if(prev == buf[index])
                counter++;
            else
                changed = DIFF_CHAR;

            if(changed == DIFF_CHAR || counter == 255)
            {
                if(counter == 1 && prev != SENTINEL)
                    fprintf(fileOut, "%c", prev);
                else
                    fprintf(fileOut, "%c%c%c", SENTINEL, prev, counter);

                prev = buf[index];
                changed = SAME_CHAR;
                counter = counter == 255 ? 0 : 1;
            }
        }
        bytes += length;
    }

    return bytes;
}

int QCompress_decodeRLE(FILE *fileIn, FILE *fileOut)
{
    int index;
    UINT bytes, length;
    char buf[BUFFER_SIZE];
    UCHAR s, c1, c2;
    UINT segment;

    segment = 2;
    bytes = 0;
    while(1)
    {
        length = fread(buf+2, sizeof(char), BUFFER_SIZE-2, fileIn);
        if(length == 0)
            break;

        index = 2;
        if(segment == 0)
            buf[--index] = s;
        if(segment == 1)
            buf[--index] = c1, buf[--index] = s;
        for(; index < (int)length+2; index++)
        {
            s = buf[index];
            if(s == SENTINEL)
            {
                segment = 0;
                if((length+2) - index > 1)
                    c1 = buf[++index], segment++;
                if((length+2) - index > 1)
                    c2 = (unsigned int)buf[++index], segment++;

                while(segment == 2 && c2-- > 0)
                    fprintf(fileOut, "%c", c1);
            }
            else
                fprintf(fileOut, "%c", s);
        }
        bytes += length;
    }

    return bytes;
}

//////////////////////////////////////////////////////////////////////////////////
#ifdef  RIGHT
#undef  RIGHT
#endif
#define RIGHT 1
#ifdef  LEFT
#undef  LEFT
#endif
#define LEFT  0
#define HUFF_NOTENCODE  -1
#define MAX_CHARNUM     256
#define MAX_HUFFTREE    (MAX_CHARNUM*2 - 1)

typedef struct HuffTreeNode_T HTreeNode;

struct HuffTreeNode_T{
    UINT   m_freq;
    char*  m_code;
    int    m_parent; //we have at most 256*2-2 nodes, short is enough
    int    m_link[2]; //the relationship is the most important to hufftree
};

typedef HTreeNode HuffTree[MAX_HUFFTREE];

static void _HuffTreeInit(HuffTree huffTree, int length)
{
    int index;
    memset(huffTree, 0, sizeof(HTreeNode)*length);
    for(index = 0; index < length; index++)
    {
        huffTree[index].m_link[LEFT]  = HUFF_NOTENCODE;
        huffTree[index].m_link[RIGHT] = HUFF_NOTENCODE;
        huffTree[index].m_parent = HUFF_NOTENCODE;
    }
}

//prev order traverse to encode, the root is 0
static void _HuffTreeEncode(HuffTree huffTree, int root)
{
    int res, parent;

    if(huffTree[root].m_parent == HUFF_NOTENCODE)//must be the root
    {
        huffTree[root].m_code = (char*)QMem_Malloc(1);
        assert(huffTree[root].m_code);
        huffTree[root].m_code[0] = '\0';
    }
    else
    {
        parent = huffTree[root].m_parent;
        if(huffTree[parent].m_link[LEFT] == root)
            res = LEFT;
        else if(huffTree[parent].m_link[RIGHT] == root)
            res = RIGHT;
        else
            assert(0);//can't be here
        huffTree[root].m_code = (char*)QMem_Malloc(strlen(huffTree[parent].m_code) + 2);
        assert(huffTree[root].m_code);//one more byte for '\0'
        sprintf(huffTree[root].m_code, "%s%d", huffTree[parent].m_code, res);

    }

    if(huffTree[root].m_link[LEFT] != HUFF_NOTENCODE)
    {
        _HuffTreeEncode(huffTree, huffTree[root].m_link[LEFT]);
        _HuffTreeEncode(huffTree, huffTree[root].m_link[RIGHT]);
    }
}

static void _HuffTreeBuild(HuffTree huffTree, int leafNums)
{
    int huffRoot, left, right;
    int rootIndex, index;

    if(leafNums <= 1)
        assert(0);//do somethins here, we have only 0 or 1 charaters

    //we have at least two nodes
    huffRoot = MAX_CHARNUM + leafNums - 2;
    for(rootIndex = MAX_CHARNUM; rootIndex <= huffRoot; rootIndex++)
    {
        for(index = 0; index < rootIndex; index++)
        {
            if(huffTree[index].m_freq && huffTree[index].m_parent == HUFF_NOTENCODE)
            {
                left = index++;
                break;
            }
        }

        for(; index < rootIndex; index++)
        {
            if(huffTree[index].m_freq && huffTree[index].m_parent == HUFF_NOTENCODE)
            {
                right = index++;
                break;
            }
        }

        leafNums = left;//just reuse leafNums here to swap left and right
        if(huffTree[left].m_freq > huffTree[right].m_freq)
            left = right, right = leafNums; //cann't be equal here, left is less than right
        for(; index < rootIndex; index++)
        {
            if(huffTree[index].m_freq && huffTree[index].m_parent == HUFF_NOTENCODE)
            {
                if(huffTree[index].m_freq < huffTree[left].m_freq)
                    right = left, left = index;
                else if(huffTree[index].m_freq < huffTree[right].m_freq)
                    right = index;
            }
        }
        //now, left is the minist, right is the second minist
        huffTree[rootIndex].m_freq = huffTree[left].m_freq + huffTree[right].m_freq;
        huffTree[rootIndex].m_link[LEFT]  = left;
        huffTree[rootIndex].m_link[RIGHT] = right;
        huffTree[left].m_parent = rootIndex;
        huffTree[right].m_parent = rootIndex;
    }
}

static void _fprintf32(FILE *file, UINT ui)
{
    unsigned char *ptr = (char*)&ui;
#if QLIB_BYTE_ORDER == QLIB_LITTLE_ENDIAN
    fprintf(file, "%c%c%c%c", ptr[3], ptr[2], ptr[1], ptr[0]);
#else //not test in big-endian machine
    fprintf(file, "%c%c%c%c", ptr[0], ptr[1], ptr[2], ptr[3]);
#endif
}

static void _fprintf16(FILE *file, USHORT us)
{
    unsigned char *ptr = (char*)&us;
#if QLIB_BYTE_ORDER == QLIB_LITTLE_ENDIAN
    fprintf(file, "%c%c", ptr[1], ptr[0]);
#else //not test in big-endian machine
    fprintf(file, "%c%c", ptr[0], ptr[1]);
#endif
}

static void _HuffCompress(FILE *fileIn, FILE *fileOut, HuffTree huffTree, int leafNums)
{
    int    index, counter, len;
    UCHAR  byte, c;

    //write the huff tree into fileOut
    fprintf(fileOut, "%c", ALGO_HUFFMAN);//Algorithm signature
    fprintf(fileOut, "%c", ((leafNums-1)&0xff));//leafs always less than 256
    for(index = 0; index < MAX_CHARNUM; index++)
        if(huffTree[index].m_freq)//donot need to store m_freq
        {
            fprintf(fileOut, "%c", (index&0xff));
            _fprintf32(fileOut, huffTree[index].m_freq);//Rebuild the hufftree with its freq when decode
        }

    //start to compress the file
    rewind(fileIn);
    byte = counter = 0;
    do
    {
        c = fgetc(fileIn);
        if(feof(fileIn))
            break;

        len = strlen(huffTree[c].m_code);
        for(index = 0; index < len; index++)
        {
            if(counter == 8)
            {
                fprintf(fileOut, "%c", byte);
                counter = byte = 0;
            }
            byte <<= (counter++ == 0 ? 0 : 1);
            byte |= huffTree[c].m_code[index]-'0';
        }
    }while(1);
    byte <<= (8-counter);
    fprintf(fileOut, "%c", byte);
    fprintf(fileOut, "%c", (counter&0xff));
}

//Just for debug when develop
static void _HuffPrint(HuffTree huffTree)
{
    int   index;

    for(index = 0; index < MAX_CHARNUM; index++)
        if(huffTree[index].m_freq)
            printf("%c:%4d %s \n", (char)(index&0xff), huffTree[index].m_freq,
                    huffTree[index].m_code);
}

static void _HuffTreeClear(HuffTree huffTree)
{
    int index;

    for(index = 0; index < MAX_HUFFTREE; index++)
        if(huffTree[index].m_code != NULL)
            QMem_Free(huffTree[index].m_code);
}

int QCompress_encodeHUFF(FILE *fileIn, FILE *fileOut)
{
    int    leafNums, huffRoot;//huffRoot is the root index in huffTree
    UINT   bytes;
    UCHAR  c;
    HuffTree huffTree; //huffman tree is a strictly binary tree

    bytes = 0;
    leafNums = MAX_CHARNUM;
    _HuffTreeInit(huffTree, MAX_HUFFTREE);
    do{
        c = fgetc(fileIn);//fgetc will be buffered automatically
        if(feof(fileIn))
            break;

        bytes++;
        if(!huffTree[c].m_freq++)
            leafNums--; //count there are how many different characters in fileIn
    }while(1);

    if(bytes <= 1)//we need at least 2 different charaters
        return QLIB_ERR_DONE;

    leafNums = MAX_CHARNUM - leafNums;
    huffRoot = MAX_CHARNUM + leafNums - 2;
    _HuffTreeBuild(huffTree, leafNums);
    _HuffTreeEncode(huffTree, huffRoot);
    _HuffPrint(huffTree);//Just for debug
    _HuffCompress(fileIn, fileOut, huffTree, leafNums);
    _HuffTreeClear(huffTree);
    return bytes;
}

int QCompress_decodeHUFF(FILE *fileIn, FILE *fileOut)
{
    UINT   freq,  tmp;
    USHORT index, leafNums;
    USHORT point, root;
    UCHAR  byte,  res;
    HuffTree huffTree;
    int cur, end, counter;

    byte = fgetc(fileIn);
    if(byte != ALGO_HUFFMAN)
        return QLIB_ERR_INVAL;

    //Reset the file pointer position is necessary, because the file can be buffered
    //in Windows or some other systems(I guess this), in this situation, you can't 
    //get the correct position of the file pointer. Reset it, then is OK.
    fseek(fileIn, 1, SEEK_SET);//I only oberserve it in Windows and MinGW
    _HuffTreeInit(huffTree, MAX_HUFFTREE);
    leafNums = fgetc(fileIn) + 1;

    for(index = 0; index < leafNums; index++)
    {
        res = fgetc(fileIn);

        freq = fgetc(fileIn);
        freq <<= 8;
        freq |= fgetc(fileIn);
        freq <<= 8;
        freq |= fgetc(fileIn);
        freq <<= 8;
        freq |= fgetc(fileIn);

        huffTree[res].m_freq = freq;
    }
    _HuffTreeBuild(huffTree, leafNums);

    //huff tree has been rebuilt, decompress the file now
    root = MAX_CHARNUM + leafNums - 2;
    point = root;

    cur = ftell(fileIn);
    fseek(fileIn, 0, SEEK_END);
    end = ftell(fileIn);
    fseek(fileIn, cur, SEEK_SET);
    counter = 0;
    do
    {
        byte = fgetc(fileIn);
        cur = ftell(fileIn);//unknow bugs in windows or other system?? I don't know...
        if((end - cur) == 1)//end is unarrivable, EOF, so here is 1
            break;

        for(index = 0; index < 8; index++)
        {
            tmp = byte & 0x80;//the most significant bit
            tmp = tmp == 0 ? 0 : 1;
            byte <<= 1;
            point = huffTree[point].m_link[tmp];//walk down
            if(point < MAX_CHARNUM)//all leafs are characters
            {
                fprintf(fileOut, "%c", (point&0xff));
                counter++;
                point = root;
            }
        }
    }while(1);

    res = fgetc(fileIn);//the length of the last byte
    for(index = 0; index < res; index++)
    {
        tmp = byte & 0x80;
        tmp = tmp == 0 ? 0 : 1;
        byte <<= 1;
        point = huffTree[point].m_link[tmp];
        if(point < MAX_CHARNUM)
        {
            fprintf(fileOut, "%c", (point&0xff));
            counter++;
            point = root;
        }
    }

    return counter;
}

///////////////////////////////////////////////////////////////////////////////////
#define LZ77_COMPRESSED  1
#define LZ77_NOTCOMPRESS 0
#define LZ77_WINDOW  2048
#define LZ77_PRELOAD 50
#define LZ77_OFFSET  11
#define LZ77_LENGTH  5 

typedef struct LZ77_Window_T  LZWindow;
typedef struct LZ77_Preload_T LZPreload;

struct LZ77_Window_T{
    char m_window[LZ77_WINDOW+1]; //one more for '\0'
};

struct LZ77_Preload_T{
    char m_preload[LZ77_PRELOAD+1]; //one more for '\0'
};
#if 0
//Just shift the first character from preload to window, then append c to preload
void _LZ77Shift(LZWindow *window, LZPreload *preload, char c)
{
    int wlen, plen;

    wlen = strlen(window->m_window);
    plen = strlen(preload->m_preload);

    if(wlen == LZ77_WINDOW)
    {
        strcpy(window->m_window, window->m_window+1, LZ77_WINDOW-1);
        wlen = LZ77_WINDOW - 1;//from length to index 
    }
    window->m_window[wlen] = preload->m_preload[0];
    window->m_window[wlen+1] = '\0';

    //preload must be full now or tailed with '\0'
    strcpy(preload->m_preload, preload->m_preload+1, plen-1);
    preload->m_preload[plen-1] = c;
    preload->m_preload[plen] = '\0';
}

//print the least significant bits of data to fileOut, bits should less than 32
static void _FprintBits(FILE *fileOut, UINT data, int bits)
{
    static int counter = 0;
    static UCHAR byte  = 0;
    int index;

    assert(bits > sizeof(UINT)*8);
    //data &= ((~0)>>(sizeof(UINT)*8-bits));//~0 here not 0xffffffff, for both 32bits and 64bits machine
    for(index = 1; index <= bits; index++)
    {
        byte |= ((data>>(index-1))&0x1)<<(7-counter));//tricky here? Not at all...
        counter++;

        if(counter == 8)
        {
            fprintf(fileOut, "%c", byte);
            byte = 0, counter = 0;
        }
    }

    if(bits == 0)//flush the last byte into fileOut
        fprintf(fileOUt, "%c", byte);
}

static void _FlushPreload(LZWindow *window, LZPreload *preload)
{
    int index;
    int offset, length;

    for(index = 0; index < bytes; )
    {
        offset = QStrprefix(window->m_window, preload->m_preload, &length);
        //LZ77Encode here
        index += length;
        while(length-- > 0)
            _LZ77Shift(window->m_window, preload->m_preload, '\0');
    }
}

int QCompress_lz77(FILE *fileIn, FILE *fileOut)
{
    char c;
    int bytes, bytes2, index;
    int pre, prelen;
    char buf[LZ77_PRELOAD];
    LZWindow  window;
    LZPreload preload;

    bytes = 0;
    pre = 0;
    prelen = 0;
    window.m_len = 0;
    preload.m_len = 0;

    memset(preload.m_preload, '\0', LZ77_PRELOAD+1);
    fread(preload.m_preload, sizeof(char), LZ77_PRELOAD, fileIn);
    while(1)
    {
        //first time, preload is full, read only one char
        offset = QStrprefix(window.m_window, preload.m_preload, &length);
        if(length <= 1)//cannot/donot compress
        {
            _FprintBits(fileOut, LZ77_NOTCOMPRESS<<31, 1);
            _FprintBits(fileOut, preload.m_preload[0], 8);
        }
        else //compress with offset, length
        {
            _FprintBits(fileOut, LZ77_COMPRESSED<<31, 1);
            _FprintBits(fileOut, );
        }
        //encode here
        memset(buf, '\0', LZ77_PRELOAD);
        bytes2 = fread(buf, sizeof(char), length, fileIn);
        for(index = 0; index < bytes2; index++)
            _LZ77Shift(&window, &preload, buf[index]);
        if(feof(fileIn))
            break;
    }
    _FlushPreload(&window, &preload);

}
#endif
