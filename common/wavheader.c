

#include "wavheader.h"




struct WaveHeader *headerCreate()
{
    struct WaveHeader *wav = malloc(sizeof(struct WaveHeader));
    return wav;
}

void printHeader(struct WaveHeader *header)
{
    int i = 0;
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
    printf("Data size: %d\n", header->datachunk_size);
    return;
}

struct WaveHeader *readHeader(FILE *fp, struct WaveHeader *header)
{

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
    fread(&header->datachunk_id, sizeof(char), 4, fp);
    fread(&header->datachunk_size, sizeof(int), 1, fp);

  //  for (i = 0; i < numOfSamples; i+=512)
  //  {
  //     readDataIntoBuffer(foo, fp);
  //  }
  }
  return header;
}

void readDataIntoBuffer(float *b, FILE *fp)
{
    int i = 0;
    short int sample = 0;
    for (i = 0; i < 512; i++)
    {
        fread(&sample, 1, 2, fp);
        b[i] = sample;
    }
    return;
}
