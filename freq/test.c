#include <stdio.h>
#include "../common/wavheader.h"
#include <time.h>
#include <fftw3.h>
#define BUFFERSIZE 140

int main(int argc, char *argv[])
{
short int foo [BUFFERSIZE];

  int N = 256;
  short int sample;
  int i = 0;
  int j = 0;
  unsigned int numOfSamples = 0;
  struct WaveHeader *wav = malloc(sizeof(struct WaveHeader));
  int samplerate = 0;
  double *in;
  double processed[N/2];
  fftw_complex * out;
  fftw_plan my_plan;

  float output[16];
  float zero=0;

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


 //  if (processData(samplerate, i, foo))
 //   break;
  }
 // return;
  in = (double*) fftw_malloc(sizeof(fftw_complex)*N);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*((N/2)+1));
  my_plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
  for (i = 0; i < numOfSamples; i=i+(N-N/2)) {
    for (j = 0; j < N; j++) {
      in[j] =  wav->chan1[(i+j)]*(0.54 - 0.46 * cos(2 * M_PI * j / N));

      //printf("%f\n", in[j]);
    }
    fftw_execute(my_plan);
    //http://ubuntuforums.org/showthread.php?t=1489837
    double m = 0.0;
    double max = 0.0;
    double correction = (double)samplerate / (double)N;
    for (j = 1; j<(N/2)+1; j++) {
      out[j][0] *=2./N;
      out[j][1] *= (2./N);
      double mag = (out[j][0]*out[j][0] + out[j][1]*out[j][1]);
      double cc = (double)m * correction;
      //mag = (mag/N)*20000;

      mag = sqrtf(mag);
      if (mag > max)
        max = cc;
      if (mag <1)
        mag = 0;
    printf("%d %f %f  \n",i,cc,(mag));
      m++;

    }

 //   printf("%d %f\n",i, max);
    //double mag = (out[0][0]*out[0][0] + out[0][1]*out[0][1]);
    //printf("%d %f\n", i, mag);
    //printf("%d 20\n", j);
  printf("\n");
  }
   //fftw_execute(my_plan);
  //

    //http://www.roxlu.com/2014/038/fast-fourier-transform




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
