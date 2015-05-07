#include "wavheader.h"


struct WaveHeader *headerCreate()
{
    struct WaveHeader *wav = malloc(sizeof(struct WaveHeader));
    return wav;
}

void printHeader(struct WaveHeader *header)
{
    unsigned int i = 0;
    for (i = 0; i < sizeof(header->chunk_id); i++)
    {
      printf("%c", header->chunk_id[i]);
    }
    printf("\n");
    printf("%d\n", header->chunk_size+8);
    printf("wav ID: ");
      for (i = 0; i < sizeof(header->chunk_id); i++)
    {
      printf("%c", header->wavID[i]);
    }
    printf("\n");
    printf("Fmt ID: ");
      for (i = 0; i < sizeof(header->chunk_id); i++)
    {
      printf("%c", header->fmtID[i]);
    }
    printf("\n");
        printf("fmt size: %d\n", header->fmtchunk_size);
    printf("format tag: %hu\n", header->wFormatTag);
    printf("N channels: %hu\n", header->nChannels);
    printf("Sample per sec: %d\n", header->sample_rate);
    printf("bytes per sec: %d\n", header->byte_rate);
    printf("blockalign: %d\n", header->block_align);
    printf("bits per sample: %d\n", header->bps);

    for (i = 0; i < sizeof(header->chunk_id); i++)
    {
      printf("%c", header->datachunk_id[i]);
    }
    printf("\r\nData size: %d\n", header->datachunk_size);
    return;
}

struct WaveHeader *readHeader(FILE *fp, struct WaveHeader *header)
{
  char name[4];
  int result;
  int cont = 0;

  if (fp)
  {
    fread(&header->chunk_id, sizeof(char), 4, fp); //read in first four bytes
    fread(&header->chunk_size, sizeof(int), 1, fp); //read in 32bit size value

    fread(&header->wavID, sizeof(char), 4, fp);
    fread(&header->fmtID, sizeof(char), 4, fp);
    fread(&header->fmtchunk_size, sizeof(unsigned int), 1, fp);
    fread(&header->wFormatTag, sizeof(unsigned short int), 1, fp);
    fread(&header->nChannels, sizeof(short int), 1, fp);
    fread(&header->sample_rate, sizeof(int), 1, fp);
    fread(&header->byte_rate, sizeof(int), 1, fp);
    fread(&header->block_align, sizeof(short int), 1, fp);
    fread(&header->bps, sizeof(short int), 1, fp);
    //data chucnk should be the next one
    //implement a catch just in case
    fread(&name, sizeof(char), 4, fp);
    while (!cont) {

      if (name[0] != 'd' || name[1] != 'a' || name[2] != 't' || name[3] != 'a') {
        printf("**%s\n", name);
        advanceToNextChunk(fp);
        fread(&name, sizeof(char), 4, fp);
      }
      else
          cont = 1;
    }
    fseek(fp,-4,SEEK_CUR);
    fread(&header->datachunk_id, sizeof(char),4,fp);
    fread(&header->datachunk_size, sizeof(int),1,fp);
  }
  return header;
}

void readDataIntoBuffer(float *b, FILE *fp)
{
    readAmountIntoBuffer(512, b, fp);
}


void advanceToNextChunk(FILE *fp)
{
  int size = 0;
  fread(&size, sizeof(int), 1, fp);
  fseek(fp,size,SEEK_CUR);
}

void readAllData(FILE *fp, struct WaveHeader *header)
{
  int i = 0;
  int numOfSamples = 0;
  numOfSamples = header->datachunk_size / (header->nChannels * (header->bps / 8));


  header->chan1 = malloc(numOfSamples  * sizeof(short int));
  if (header->nChannels > 1)
    header->chan2 = malloc((numOfSamples  * sizeof(short int)));
  if (header->nChannels > 2)
    header->chan3 = malloc((numOfSamples  * sizeof(short int)));
  if (header->nChannels > 3)
    header->chan4 = malloc((numOfSamples  * sizeof(short int)));

  for (i = 0; i < header->datachunk_size / header->block_align; i++) {
    fread(&header->chan1[i],1,2,fp);
    if (header->nChannels > 1)
      fread(&header->chan2[i],1,2,fp);
    if (header->nChannels > 2)
      fread(&header->chan3[i],1,2,fp);
    if (header->nChannels > 3)
      fread(&header->chan4[i],1,2,fp);
  }
}

void readAmountIntoBuffer(int samples, float *b, FILE *fp)
{
    int i = 0;
    short int sample = 0;
    for (i = 0; i < samples; i++)
    {
        fread(&sample, 1, 2, fp);
        b[i] = sample;
    }
    return;
}
