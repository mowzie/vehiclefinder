#include <stdio.h>
#include "../common/wavheader.h"
#include <time.h>



int main(int argc, char *argv[])
{
  int numOfSamples = 0;
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

  free(wav);

  processData(samplerate, numOfSamples, fp);
  fclose(fp);
  return 0;
}

