#include <stdio.h>
#include <math.h>
#include <stdlib.h>
struct WaveHeader{
    char               chunk_id[4];
    int                chunk_size;
    char               wavID[4];
    char               fmtID[4];
    unsigned short int fmtchunk_size;
    unsigned short int wFormatTag;
    short int         nChannels;
    int               sample_rate;
    int               byte_rate;
    short int         block_align;
    short int         bps;
    char               datachunk_id[4];
    unsigned int      datachunk_size;
    short int * chan1;
    short int * chan2;
    short int * chan3;
    short int * chan4;
};


void readDataIntoBuffer(float *b, FILE *fp);
void readAmountIntoBuffer(int samples, float *b, FILE *fp);
struct WaveHeader *readHeader(FILE *fp, struct WaveHeader *header);
void printHeader(struct WaveHeader *header);
void advanceToNextChunk(FILE *fp);
void readAllData(FILE *fp, struct WaveHeader *header);
void freeChannelMemory(struct WaveHeader *wav);
