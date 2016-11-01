//
/* Run-length encoding(RLE) is a very simple form of data compression in which runs of data
 * (that is, sequence in which the same value occurs in many consecutive data elements) are
 * stored as a single data value and count, rather than as the original run. This is most
 * useful on data that contains many such runs. Consider, for example, simple graphic images
 * such as icons, line drawings, and animations. It is not useful with files that don't have
 * many runs as it could greatly increase the file size.
 * RLE example:
 *    WWWWWWWWWWBWWWBBBBBWWWW  =>   10W1B3W5B4W, or more simple as W10BW3B5W4
 * Actually, if the length of run is less than 2, we can benefit nothing from RLE encoding,
 * so we only need to encode the character when it occurs consecutively more that twice.
 * But what about this case:
 *    1111122223454444, we can't identify the character and its counter, we need a identify
 * character. We call it Sentinel here. Then, the RLE encoding is S15S24345S44
 */


#ifndef _QLIB_COMPRESS_H_
#define _QLIB_COMPRESS_H_

int QCompress_encodeRLE(FILE *fileIn, FILE *fileOut);
int QCompress_decodeRLE(FILE *fileIn, FILE *fileOut);
int QCompress_encodeHUFF(FILE *fileIn, FILE *fileOut);
int QCompress_decodeHUFF(FILE *fileIn, FILE *fileOut);

#endif //!_QLIB_COMPRESS_H_
