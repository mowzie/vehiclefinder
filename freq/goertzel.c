#include <math.h>
#include <stdio.h>
#define BUFFERSIZE 128

float goertzel(int numSamples,int TARGET_FREQUENCY,int SAMPLING_RATE, float* data);
void processData(int, int, FILE * fp);


float goertzel(int numSamples,int TARGET_FREQUENCY,int SAMPLING_RATE, float* data)
{
    int     k,i;
    float   floatnumSamples;
    float   omega = 0;
    float sine = 0;
  float cosine = 0;
  float coeff = 0;
  float q0 = 0;
  float q1 = 0;
  float q2 = 0;
  float result = 0;
  float real = 0;
  float imag = 0;
  //  SAMPLING_RATE *=2;
    floatnumSamples = (float) numSamples;
    k = (int) ( ((floatnumSamples * TARGET_FREQUENCY) / SAMPLING_RATE));

  //k =   k = (int) (floatnumSamples/ 10)*TARGET_FREQUENCY / SAMPLING_RATE;

  omega = (2.0 * M_PI *k) / floatnumSamples;

  sine = sin(omega);    //imag
    cosine = cos(omega);
    coeff = 2.0 * cosine; //real
    q0=0;
    q1=0;
    q2=0;

    for(i=1; i<numSamples; i++)
    {
      q1 += (double)data[i] * cosine;
      q2 += (double)data[i] * sine;

      data[i] *= 0.54 - 0.46 * cos(2 * M_PI * i / numSamples);
      q0 = data[i] + coeff * q1 - q2;
      q2 = q1;
        q1 = q0;
    }

    real = (q1 - q2 * cosine) / (numSamples / 2.0);
  imag = (q2 * sine) / (numSamples / 2.0);
   // real = 0.5 * coeff*q1 - q2;
   // imag = sine*q2;

  result = sqrtf(real*real + imag*imag);
//  result = real*real + imag*imag;
    return result;
}


void processData(int freq, int numOfSamples, FILE *fp) {
  //int test = 0;
  short int sample = 0;
  int i = 0;
  int j = 0;
  float first = 0.0;
  float second = 0.0;
  float power = 0.0;
  int count = 0;
  int last = 0;
  int wail = 0;
  int wailC = 0;
  int hilo = 0;
  //int hiloC = 0;
  float foo [BUFFERSIZE];


  for (i = 0; i < numOfSamples; i+=BUFFERSIZE) {
    for (j = 0; j < BUFFERSIZE; j++) {
      fread(&sample,1,2,fp);
      foo[j] = sample;
    }
    /*
    printf("%d 530 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,530,freq, foo));
    printf("%d 535 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,535,freq, foo));
    printf("%d 540 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,540,freq, foo));
    printf("%d 545 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,545,freq, foo));
    printf("%d 550 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,550,freq, foo));
    printf("%d 555 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,555,freq, foo));
    printf("%d 560 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,560,freq, foo));
    printf("%d 565 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,565,freq, foo));
    printf("%d 570 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,570,freq, foo));
    printf("%d 575 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,575,freq, foo));
    printf("%d 580 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,580,freq, foo));
    printf("%d 585 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,585,freq, foo));
    printf("%d 590 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,590,freq, foo));
    printf("%d 595 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,595,freq, foo));
    printf("%d 600 %f \n", i+BUFFERSIZE, goertzel(BUFFERSIZE,600,freq, foo));
    */
//960
    first = goertzel(BUFFERSIZE,200,freq, foo);
    second = goertzel(BUFFERSIZE,960,freq, foo);
    if (i != 0) {
      if (first > 6e05) {
          if (second > 1e06) {
            //printf("Fire Truck Horn\n");
            //return;
          }
          //else
      }

///////////////////////
//High Low
//////////////////////

      if (hilo == 0) {
        //600
        power = goertzel(BUFFERSIZE,600,freq, foo);
        //if (power > 1000) {
          //printf("%d %f low \n", i+BUFFERSIZE, power);
          //hilo = 1;
        //}
      }
      //else {
        power = goertzel(BUFFERSIZE,1500,freq, foo);
        //if (power > 800) {
        //printf("%d %f high \n", i+BUFFERSIZE, power);
        count++;
        last = 0;
        hilo = 0;
      //}
      //else
          last=last + BUFFERSIZE;
      //}
      if (last > 2560) {
        last = 0;
      }
      if (count > 15) {
      //printf("High Low Siren\n");
      //return;
      }

//////////////////////////////////////

///////////////////////////////////
//
//Search for wailing  (oscilations between 620hz -> 1500 hz)
//
///////////////////////////////////
//600
      power = goertzel(BUFFERSIZE,600,freq, foo);
      //if (power < 10)
        //power = 0;
      printf("%d %f", i+BUFFERSIZE, power);
      power = goertzel(BUFFERSIZE,1300,freq, foo);
      //if (power < 10)
        //power = 0;
      printf(" %f \n", power);
      if (wail == 0) {
        power = goertzel(BUFFERSIZE,620,freq, foo);
        if (power > 1e05) {
          wail = 1;
        }
      }
      else {
        power = goertzel(BUFFERSIZE,1500,freq, foo);
        if (power > 1e05) {
          wailC++;
          last = 0;
          wail = 0;
        }
        else
          last=last + BUFFERSIZE;
      }
      if (last > 2560) {
        last = 0;
      }
      if (wailC > 15) {
        //printf("Wailing Siren\n");
        //return;
      }

//////////////////////////////////////
//End Wail search
//////////////////////////////////////

    }
  }
}
