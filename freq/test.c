#include <stdio.h>
#include "../common/wavheader.h"
#include <time.h>



int main(int argc, char *argv[])
{
  short int *chan1;
  short int sample;
  int i = 0;
  unsigned int numOfSamples = 0;
  struct WaveHeader *wav = malloc(sizeof(struct WaveHeader));
  int samplerate = 0;
  FILE * fp;

  fp = fopen(argv[1], "rb");
  if (!fp)
    return 0;

  readHeader(fp, wav);
  //printHeader(wav);

  samplerate = wav->sample_rate;
  numOfSamples = wav->datachunk_size / (wav->nChannels * (wav->bps / 8));

  //code expects mono channel, read in one channel and store into array
  chan1 = malloc(numOfSamples  * sizeof(short int));
  for (i = 0; i < numOfSamples; i++) {
    fread(&sample,1,2,fp);
    chan1[i] = sample;
  }

  //don't need the header anymore
  free(wav);
  //process the data
  processData(samplerate, numOfSamples, chan1);
  free(chan1);
  fclose(fp);
  return 0;
}

