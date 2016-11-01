
#include <stdio.h>
#include <assert.h>
#include "qlib/qcompress.h"
#include "qlib/qgeneral.h"

void rletest()
{
    FILE *fileIn, *fileOut, *deOut;

    printf("In RLE compress decompress test.\n");
    fileIn = fopen("words.txt", "r");
    assert(fileIn);
    fileOut = fopen("enrle.txt", "w+");
    assert(fileOut);
    deOut = fopen("derle.txt", "w+");

    QCompress_encodeRLE(fileIn, fileOut);
    fclose(fileIn);
    fseek(fileOut, 0, SEEK_SET);//rewind(fileOut);
    QCompress_decodeRLE(fileOut, deOut);
    fclose(deOut);
    fclose(fileOut);
}

void huffmantest()
{
    FILE *fileIn, *fileOut, *fileDe;
    int res;

    printf("In huffman compress decompress test.\n");
    //Please open all the files in binary mode, or there would be errors
    //This is mainly because the enter character is different in different system
    fileIn = fopen("PhotosynthControls.dll", "rb");
    assert(fileIn);
    fileOut = fopen("enhuffwords.txt", "wb+");
    assert(fileOut);
    fileDe = fopen("dehuffwords.dll", "wb+");
    assert(fileDe);

    res = QCompress_encodeHUFF(fileIn, fileOut);
    assert(res != QLIB_ERR_DONE);//there should be at least 2 charaters in fileIn
    printf("Compress file: %d bytes\n", res);
    fclose(fileOut);
    fileOut = fopen("enhuffwords.txt", "rb");
    res = QCompress_decodeHUFF(fileOut, fileDe);
    printf("Decompressed file: %d bytes\n", res);
    fclose(fileIn);
    fclose(fileOut);
    fclose(fileDe);
}

int main()
{
    rletest();
    huffmantest();

    QMemDumpInfo(0);
    return 0;
}
