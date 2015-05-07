#include <stdio.h>
#include "../common/wavheader.h"
#include <time.h>



int main(int argc, char *argv[])
{

  short int sample;
  int i = 0;
  unsigned int numOfSamples = 0;
  struct WaveHeader *wav = malloc(sizeof(struct WaveHeader));
  int samplerate = 0;
  FILE * fp;
  FILE * fOut;

  fp = fopen(argv[1], "rb");
  fOut = fopen("foo.wav", "wb");

  if (!fOut)
    return 0;

  if (!fp)
    return 0;

  readHeader(fp, wav);
  //printHeader(wav);

  samplerate = wav->sample_rate;
  numOfSamples = wav->datachunk_size / (wav->nChannels * (wav->bps / 8));

  readAllData(fp,wav);

  //process the data
  processData(samplerate, numOfSamples, wav->chan1);
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


  fclose(fp);
  fclose(fOut);
  return 0;
}

