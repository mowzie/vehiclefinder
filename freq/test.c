#include <stdio.h>
#include "../common/wavheader.h"
#include <time.h>
#define BUFFERSIZE 140


int main(int argc, char *argv[])
{
short int foo [BUFFERSIZE];
  short int sample;
  int i = 0;
  int j = 0;
  unsigned int numOfSamples = 0;
  struct WaveHeader *wav = malloc(sizeof(struct WaveHeader));
  int samplerate = 0;
  FILE * fChan1;
  FILE * fChan2;
  FILE * fChan3;
  FILE * fChan4;

  FILE * fOut;

  fChan1 = fopen(argv[1], "rb");
  //fChan1 = fopen(argv[2], "rb");
  //fChan1 = fopen(argv[3], "rb");
  //fChan1 = fopen(argv[4], "rb");



  if (!fChan1)
    return 0;

  readHeader(fChan1, wav);
  //printHeader(wav);

  samplerate = wav->sample_rate;
  numOfSamples = wav->datachunk_size / (wav->nChannels * (wav->bps / 8));

  readAllData(fChan1,wav);

  //process the data
  for (i = 0; i < numOfSamples; i+=(BUFFERSIZE-(BUFFERSIZE/2))) {
    for (j = 0; j < BUFFERSIZE; j++) {
      if ((i+j) < numOfSamples)
        foo[j] = wav->chan1[i+j];
    }


  if (processData(samplerate, i, foo))
    break;
  }
  /*
  //test code to write out raw data as it was read in
  for (i = 0; i < numOfSamples; i++) {
    fwrite(&wav->chan1[i],1,2,fOut);
    if (wav->nChannels > 1)
      fwrite(&wav->chan2[i],1,2,fOut);
    if (wav->nChannels > 2)
      fwrite(&wav->chan3[i],1,2,fOut);
    if (wav->nChannels > 3)
      fwrite(&wav->chan4[i],1,2,fOut);
  }
  */
  //don't need the header anymore
  freeChannelMemory(wav);
  free(wav);


  fclose(fChan1);

  return 0;
}

