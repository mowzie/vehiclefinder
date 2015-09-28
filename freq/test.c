#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../common/wavheader.h"  //local file
#include "../timedifference/correlation.h"
#include "../freq/goertzel.h"
#include <fftw3.h>      //3rd party library, needs to be installed
#define BUFFERSIZE 512

void showUsage();
int writeGpScript(char *datName, int rate, int);
int writeFFT(int N, int,struct WaveHeader *wav);
int CalculateDirection(int N, int i, struct WaveHeader *wav);
int sirenDetect(double* X, int N);
//-----------------------------------------------------------------------------
//   Function:    main()
//
//   Description: Opens wav file up and performs FFT
//
//   Programmer:  Ian Littke
//
//   Date:        05/10/2015
//
//   Version:     1.0
//
//   Environment: Ubuntu 10.4
//                Software: gcc 4.8.2
//
//   Parameters:  -file <file.wav>  file to read from
//                -spec <N>         calculate FFT with N samples
//                -showhead         display header info
//
//   Calls:       showUsage()
//                readHeader()
//                printHeader()
//                readAllData(FILE *,struct WaveHeader)
//                writeFFT(char*,int,struct WaveHeader)
//                writeGpScript(char*,rate)
//                freeChannelMemory()
//
//   Returns:     0 (success)
//                1 (fail)
//
//   History Log:
//             05-10-2015  IL completed version 1
//             05-11-2015  IL seperated code, added usage info
//
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  int i = 0;
  int j = 0;
  unsigned int N = 0;
  int spec = 0;
  int siren = 0;
  int showHead = 0;
  char fileName[300];
  struct WaveHeader *wav;
  FILE *fWavIn = NULL;
  //FILE *fOut = NULL;
  double foo [BUFFERSIZE];

  double test[64];
  int detect;
  int counter = 0;

  if (argc < 3) {
    showUsage();
    return 1;
  }

  for (i = 1; i < (argc); i++) {
    if (strcasecmp("-spec", argv[i]) == 0) {
      spec = 1;
      if (i + 1 == argc) {
        fprintf(stderr, "\n**ERROR: No sample size associated with \"-spec\"**\n");
        return 1;
      }
      N = atoi(argv[++i]);
      if (N < 5) {
        fprintf(stderr, "\n**ERROR: <%s> is not a valid sample size (use 5+)**\n", argv[i]);
        return 1;
      }
      continue;
    }
    if (strcmp("-?", argv[i]) == 0) {
      showUsage();
      return 1;
    }
    if (strcasecmp("-siren", argv[i]) == 0) {
      siren = 1;
      continue;
    }
    if (strcasecmp("-showhead", argv[i]) == 0) {
      showHead = 1;
      continue;
    }
    if (strcasecmp("-file", argv[i]) == 0) {
      //require a filename to go along with the -file
      //check -file isn't the last arg, otherwise it throws a sigsegv
      if (i + 1 == argc) {
        fprintf(stderr, "\n**ERROR: No filename listed with \"-file\"\n");
        return 1;
      }
      strncpy(fileName, argv[++i], 300);
      fWavIn = fopen(fileName, "rb");
      if (!fWavIn) {
        fprintf(stderr, "\n**ERROR: Could not open file \"%s\"\n", fileName);
        return 1;
      }
      continue;
    }
  }

  //it's a little hacky, but this checks if we have a file
  if (fWavIn == NULL) {
    showUsage();
    return 1;
  }
  wav = malloc(sizeof(struct WaveHeader));

  strncpy(wav->wavName, fileName, 300);
  if (readHeader(fWavIn, wav) != 0) {
    fprintf(stderr, "Error reading (%s), check if it is a valid wav file\n", fileName);
    return 1;
  }
  if (showHead == 1) {
    printHeader(wav);
  }
  readAllData(fWavIn, wav);

  //process the data
  //Check for a siren
  if (siren) {
    for (i = 0; i < wav->totalSamples; i += (BUFFERSIZE - (BUFFERSIZE / 2))) {
      for (j = 0; j < BUFFERSIZE; j++) {
        if ((i + j) < wav->totalSamples) {
          foo[j] = wav->chan1[i + j]*(0.54 - 0.46 * cos(2 * M_PI * i / BUFFERSIZE));
        }
      }

      //Processes with Goertzel algorithm


      if (i > 5) {
        //printf ("\n%d\n", i);
        test[counter] = 44100.0 / getTimeDomainAC(foo, BUFFERSIZE, 20);
        counter++;
        if (counter == 64) {
        //  printf("%d\n", i);
        //  for (j = 0; j < 64; j++) {
        //    printf("%d %f\n", j, test[j]);
        //  }
        //  return 0;
          if (detect = sirenDetect(test, 64)) {
            printf("siren detected at sample: %d\n", i);
              //printf("siren detected, running direction\n");
            //CalculateDirection(BUFFERSIZE, i + detect, wav);
            //return 0;
          }
          counter = 0;
        }
        //if (i > 383747)
        //  return 0;

        continue;
        if (processData(wav->sampleRate, i, foo)) {
          if (spec == 1) {
            CalculateDirection(N, i, wav);
          }
        }
      }
    }
  }

  if (spec == 1) {
    writeFFT(N, 0, wav);
  }

  //free up memory
  freeChannelMemory(wav);
  free(wav);
  fclose(fWavIn);
  return 0;
}

//-----------------------------------------------------------------------------
//   Function:    showUsage()
//
//   Description: Shows the usage for the app
//
//   Programmer:  Ian Littke
//
//   Date:        05/10/2015
//
//   Version:     1.0
//
//   Environment: Ubuntu 10.4
//                Software: gcc 4.8.2
//
//   Returns:     0 (success)
//                1 (fail)
//
//   History Log:
//             05-10-2015  IL completed version 1
//             05-11-2015  IL seperated code, added usage info
//
// ----------------------------------------------------------------------------
void showUsage() {
  printf("Usage: ./wavInfo -file <file.wav> [-spec <N>] [-showHead]\n");
  printf("       <file.wav> can be up to 4 channels\n");
  printf("       -spec writes FFT data to <file.wav.dat> and generates gnuplot script\n");
  printf("         <N> is the samplesize for the FFT, for best results use 2^N\n");
  printf("           dat file is written in same location as the wav file\n");
  printf("           gnuplot script is written as \"spec.gp\" in the same folder as \"wavInfo\"\n");
  printf("       -showHead displays the pertinant header information (assuming wav file)\n");
  printf("\n");
}

int sirenDetect(double* X, int N) {
  int state = 0;
  int i;
  double max = X[0];
  int max_t = 0;
  int min_t = 0;
  double min = X[0];
  double previous = 0;
  double current = 0;
  int difference = 0;
  int stateChange = 0;
  double peakL = 0;
  double peakH = 0;

  for (i = 1; i < N; i++) {
    previous = X[i-1];
    current = X[i];
    if (current > max) {
      max = current;
      max_t = i;
    }
    if (current < min) {
      min = current;
      min_t = i;
    }

  //Check state for a yelping signal
    if (state == 0) {
      if (current < previous) {
        state = -1;
      }
      else
      if (current > previous) {
          state = 1;
      }
    }
    else
    if ((state == 1) && (previous > 1400)){
      if (current < previous)
      {
        state = -1;
        peakH = previous;
        max_t = i-1;
        stateChange++;
      }
    }
    else
    if ((state == -1) && (previous < 850)){
      if (current > previous) {
        state = 1;
        peakL = previous;
        min_t = i -1;
        stateChange++;
      }
    }

    if ((abs(peakH-peakL) > 600) && (abs(peakH-peakL)<800)) {
      if ((abs(max_t - min_t) > 5) && (abs(max_t - min_t) < 7)){
        if (stateChange > 4) {
        //  printf("********siren*********\n");
        //  for (t = 0; t < i; t++) {
        //    printf("%d %f\n", t, X[t]);
        //  }
        printf("yelp\n");
          return i;
        }
      }
    }
  }
  difference = floor(abs(max - min));
  if ((difference > 40) && (difference < 130)) {
    if (floor((difference / N) <= 2) || (difference / N == 9)) {
     printf("wail\n");
        return 1;
    }
  }
return 0;
}

int CalculateDirection(int N, int i, struct WaveHeader *wav) {
  int j;
  double xdelay = 0.0;
  double ydelay = 0.0;
  double in1[N];
  double in2[N];
  double in3[N];
  double in4[N];
  double top1 = 0;
  double top2 = 0;
  double a1=0, a1s=0, a2=0, a2s=0, b1s=0, b2s=0;
  double val = 180.0 / M_PI;
  double x = 0.0,y=0.0;
  double ret=0.0;

  for (j = 0; j < N; j++) {
    if ((i + j) < wav->totalSamples) {
      in1[j] = wav->chan1[i +j];
      in2[j] = wav->chan2[i+j];
      in3[j] = wav->chan3[i+j];
      in4[j] = wav->chan4[i+j];
    }
  }
  xdelay = getTimeDomainCC(in1,in3,N);
  ydelay = getTimeDomainCC(in2,in4,N);

  printf("xdelay: %f\n", xdelay);
  printf("ydelay: %f\n", ydelay);

  a1 = (xdelay * 340.) / 2;
  a2 = (ydelay * 340.) / 2;

  a1s = (a1)*(a1);
  a2s = (a2)*(a2);
  //printf("\nx degree: %f\n", asin(a1*2)*val);
  //printf("y degree: %f\n", asin(a2*2)*val);

  b1s = (fabs((1./4) - (a1s)));
  b2s = (fabs((1./4) - (a2s)));

  double bottom = ((b1s * b2s)-(a1s*a2s));
  top1 = fabs((b1s + a2s)*(a1s * b2s));
//    printf("b2s + a2s:%f a2s * b2s:%f\n", b1s +a2s, a1s*b2s);
  top2 = fabs((b2s + a1s)*(b1s * a2s));
  x = sqrtf(top1 / bottom);
  y = sqrtf(top2 / bottom);
//  printf("top1:%f top2:%f\n", top1, top2);
//  printf("bottom:%f\n", bottom);

  if (xdelay < 0)
    x = x*-1.;
  if (ydelay < 0)
    y = y*-1.;

  ret = atan2(y,x) * val;
  printf("\nSample: %d\n", i);
  //printf("delay (x,y): %f %f\n", xdelay, ydelay);
  //printf("(x,y):       %f %f\n", x, y);
  //printf("weight (x,y):%f %f\n", max13, max24);
  //printf("a1: %f a2: %f\n", a1, a2);
  //printf("a1s: %f a2s:%f\n", a1s, a2s);
  //printf("b1: %f b2: %f\n", b1, b2);
  //printf("b1s: %f b2s:%f\n", b1s, b2s);
  //printf("x: %f y:%f\n", x, y);

  printf("\nSound is at %f degrees\n", ret);
  printf("Sound is at %f degrees\n", atan(y/x)*val);

  printf("\nSound is at %f degrees\n", ret-45);
  printf("Sound is at %f degrees\n", atan(y/x)*val-45);

  return 0;
}

int writeFFT(int N, int startRead, struct WaveHeader *wav) {
  double *in1;
  double *in2;
  double *in3;
  double *in4;
  int i, j;
  double mag1 = 0.0;
  double mag2 = 0.0;
  double mag3 = 0.0;
  double mag4 = 0.0;
  double freqBin = 0.0;
  double correction = 0.0;
  fftw_complex * out1;
  fftw_complex * out2;
  fftw_complex * out3;
  fftw_complex * out4;
  fftw_plan my_plan1;
  fftw_plan my_plan2;
  fftw_plan my_plan3;
  fftw_plan my_plan4;
  FILE * fDatOut;

  int nameLength = 0;

  char datFilename[300];

  //add ".dat" to the end of the file
  //will fix later to rename ".wav"
  nameLength = sizeof(wav->wavName) / sizeof(wav->wavName[0]);
  snprintf(datFilename, nameLength + 4, "%s.dat", wav->wavName);
  fDatOut = fopen(datFilename, "w");

  correction = (double)wav->sampleRate / (double)N;

//This could be more optimized
// by making the FFT portion its seperate function and passing in the wav channel
// it would save memory by not needing 4 different plans to execute
  in1 = (double*)fftw_malloc(sizeof(double) * N);
  out1 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * ((N / 2) + 1));
  my_plan1 = fftw_plan_dft_r2c_1d(N, in1, out1, FFTW_MEASURE);

  in2 = (double*)fftw_malloc(sizeof(double) * N);
  out2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * ((N / 2) + 1));
  my_plan2 = fftw_plan_dft_r2c_1d(N, in2, out2, FFTW_MEASURE);

  in3 = (double*)fftw_malloc(sizeof(double) * N);
  out3 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * ((N / 2) + 1));
  my_plan3 = fftw_plan_dft_r2c_1d(N, in3, out3, FFTW_MEASURE);

  in4 = (double*)fftw_malloc(sizeof(double) * N);
  out4 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * ((N / 2) + 1));
  my_plan4 = fftw_plan_dft_r2c_1d(N, in4, out4, FFTW_MEASURE);

//  printf("Calcuting FFT, placing dat file at \'%s\'\n", datFilename1);



  //Iterate through the entire wav file, overlap & add by buffer size
  for (i = startRead; i < wav->totalSamples; i = i + (N - N / 2)) {
    for (j = 0; j < N; j++) {
      if ((i + j) < wav->totalSamples) {
        in1[j] = wav->chan1[i +j]*(0.54 - 0.46 * cos(2 * M_PI * j / BUFFERSIZE));
        in2[j] = wav->chan2[i+j]*(0.54 - 0.46 * cos(2 * M_PI * j / BUFFERSIZE));
        in3[j] = wav->chan3[i+j]*(0.54 - 0.46 * cos(2 * M_PI * j / BUFFERSIZE));
        in4[j] = wav->chan4[i+j]*(0.54 - 0.46 * cos(2 * M_PI * j / BUFFERSIZE));
      }
    }


    //Perform FFT for the sample size
    fftw_execute(my_plan1);
    fftw_execute(my_plan2);
    fftw_execute(my_plan3);
    fftw_execute(my_plan4);
    //Iterate through sample size
    //skip the first element, since it is the average of the sample
    for (j = 1; j<(N/2)+1; j++) {
      mag1 = sqrtf(2*(out1[j][0]*out1[j][0] + out1[j][1]*out1[j][1])/N);
      mag2 = sqrtf(2*(out2[j][0]*out2[j][0] + out2[j][1]*out2[j][1])/N);
      mag3 = sqrtf(2*(out3[j][0]*out3[j][0] + out3[j][1]*out3[j][1])/N);
      mag4 = sqrtf(2*(out4[j][0]*out4[j][0] + out4[j][1]*out4[j][1])/N);
      
      //Set the frequency bin
      freqBin = (double)(j) * correction;

      //Print out (sample FreqBin dB (unscaled))
      //This can be used with gnuplot or matlab to generate spectrogram
      fprintf(fDatOut,"%d %f %f %f %f %f\n",i,freqBin,mag1, mag2, mag3, mag4);
    }
    //gnuplot requires a line break between sample sets for spectrograms
    fprintf(fDatOut,"\n");
  }

  fftw_free(in1);
  fftw_free(out1);
  fftw_destroy_plan(my_plan1);

  fftw_free(in2);
  fftw_free(out2);
  fftw_destroy_plan(my_plan2);

  fftw_free(in3);
  fftw_free(out3);
  fftw_destroy_plan(my_plan3);

  fftw_free(in4);
  fftw_free(out4);
  fftw_destroy_plan(my_plan4);

  fftw_cleanup();
  fclose(fDatOut);
//  fclose(fDatOut2);
//  fclose(fDatOut3);
//  fclose(fDatOut4);

  //write gnuplot script
//  printf("Writing spec.gp\n");
  writeGpScript(datFilename, wav->sampleRate, i);

  return 0;
}


int writeGpScript(char *datName, int rate, int N) {
  int i = rate/2;
  FILE *gnuplotFile = fopen("spec.gp", "w");
/*
  fprintf(gnuplotFile, "########\n");
  fprintf(gnuplotFile, "DATFILE = \"%s\"\n", datName);
  fprintf(gnuplotFile, "set yrange[0:%d] #top frequency to show\n", rate / 2);
  fprintf(gnuplotFile, "#set xrange[0:]  #number of seconds to display\n");
  fprintf(gnuplotFile, "########\n");
  fprintf(gnuplotFile, "set terminal png enhanced size 800,600\n");
  fprintf(gnuplotFile, "set view 0,0\n");
  // fprintf(gnuplotFile, "set contour base\n");
  fprintf(gnuplotFile, "set lmargin at screen 0.15\n");
  fprintf(gnuplotFile, "set rmargin at screen 0.85\n");
  fprintf(gnuplotFile, "set bmargin at screen 0.15\n");
  fprintf(gnuplotFile, "set tmargin at screen 0.85\n");
  fprintf(gnuplotFile, "set title DATFILE\n");
  fprintf(gnuplotFile, "set output DATFILE[:strlen(DATFILE)-4].\'.png\'\n");
  fprintf(gnuplotFile, "set ytics rotate\n");
  fprintf(gnuplotFile, "set xlabel \"Time (s)\" offset character 0, 2,0\n");
  fprintf(gnuplotFile, "###\n");
  fprintf(gnuplotFile, "###I couldn't get the label to show =(\n");
  fprintf(gnuplotFile, "###set label 2 \"Frequency (Hz)\"\n");
  fprintf(gnuplotFile, "###\n");
  fprintf(gnuplotFile, "show label\n");
  fprintf(gnuplotFile, "set ytics border mirror\n");
  fprintf(gnuplotFile, "unset ztics\n");
  fprintf(gnuplotFile, "set grid      #Put gridlines on the plot\n");
  //fprintf(gnuplotFile,"set format cb \"\%10.1\f\"\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "set cbtics border in scale 0,0 autojustify mirror offset -8,0\n");
  fprintf(gnuplotFile, "set colorbox user\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "set colorbox vertical user origin 0.1, 0.29 size 0.01,0.45\n");
  fprintf(gnuplotFile, "#set logscale y 2\n");
  fprintf(gnuplotFile, "color(x) = x    #this just scales down the colorbox numbers\n");
  //fprintf(gnuplotFile,"#set yrange[500:2000]\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "splot DATFILE using ($1/%d):2:(color($3)) with pm3d notitle\n", rate);
  fclose(gnuplotFile);
  */

  fprintf(gnuplotFile, "#!/usr/bin/gnuplot\n");
  fprintf(gnuplotFile, "########\n");
  fprintf(gnuplotFile, "DATFILE = \"%s\"[2:strlen(\"%s\")]\n",datName,datName);
  fprintf(gnuplotFile, "set yrange[300:%d] #top frequency to show\n",rate/2);
  fprintf(gnuplotFile, "set xrange[0:%d]  #number of seconds to display\n", rate/N);
  fprintf(gnuplotFile, "########\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "set terminal png enhanced size 800,600\n");
  fprintf(gnuplotFile, "set output DATFILE[1:strlen(DATFILE)-4].'.png'\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "#Setup Plot Area\n");
  fprintf(gnuplotFile, "set multiplot layout 5,1\n");
  fprintf(gnuplotFile, "set grid front\n");
  fprintf(gnuplotFile, "unset grid\n");
  fprintf(gnuplotFile, "unset ztics\n");
  fprintf(gnuplotFile, "set ytics out mirror\n");
  fprintf(gnuplotFile, "set ytics (%d",i/=2);
  while (i > 250) {
    fprintf(gnuplotFile, ",%d", i /= 2);
  }

  fprintf(gnuplotFile, ")\n");
  fprintf(gnuplotFile, "set view 0,0  #top down view for spectrogram\n");
  fprintf(gnuplotFile, "TOP = 0.98\n");
  fprintf(gnuplotFile, "DY = 0.22\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "#Setup Labels\n");
  fprintf(gnuplotFile, "set label 1 at screen 0.87,0.63 \"Frequency (Hz)\" rotate by 270 front\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "set xtics axis nomirror\n");
  fprintf(gnuplotFile, "set format y \"\%%g\"\n");
  fprintf(gnuplotFile, "set log y\n");
  fprintf(gnuplotFile, "set colorbox user\n");
  fprintf(gnuplotFile, "unset cbtics\n");
  fprintf(gnuplotFile, "set colorbox vertical user size 0.01,0.4 origin 0.18,0.35\n");
  fprintf(gnuplotFile, "color(x) = x*2    #this just scales down the colorbox numbers\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "set label 2 at screen 0.22,0.18 \"Mic 1\" rotate by 90 front\n");
  fprintf(gnuplotFile, "set label 3 at screen 0.22,0.4  \"Mic 2\" rotate by 90 front\n");
  fprintf(gnuplotFile, "set label 4 at screen 0.22,0.61 \"Mic 3\" rotate by 90 front\n");
  fprintf(gnuplotFile, "set label 5 at screen 0.22,0.84 \"Mic 4\" rotate by 90 front\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "set xlabel \"Time (s)\" offset 0,-7\n");
  fprintf(gnuplotFile, "set tmargin at screen TOP-3*DY\n");
  fprintf(gnuplotFile, "set bmargin at screen TOP-4*DY\n");
  fprintf(gnuplotFile, "\n");
  fprintf(gnuplotFile, "splot \"1\".DATFILE using ($1/%d):2:(color($3)) with pm3d notitle\n", rate);
  fprintf(gnuplotFile, "unset xtics\n");
  fprintf(gnuplotFile, "set tmargin at screen TOP-2*DY\n");
  fprintf(gnuplotFile, "set bmargin at screen TOP-3*DY\n");
  fprintf(gnuplotFile, "splot \"2\".DATFILE using ($1/%d):2:(color($3)) with pm3d notitle\n", rate);
  fprintf(gnuplotFile, "unset xlabel\n");
  fprintf(gnuplotFile, "set tmargin at screen TOP-DY\n");
  fprintf(gnuplotFile, "set bmargin at screen TOP-2*DY\n");
  fprintf(gnuplotFile, "splot \"3\".DATFILE using ($1/%d):2:(color($3)) with pm3d notitle\n", rate);
  fprintf(gnuplotFile, "set tmargin at screen TOP\n");
  fprintf(gnuplotFile, "set bmargin at screen TOP-DY\n");
  fprintf(gnuplotFile, "splot \"4\".DATFILE using ($1/%d):2:(color($3)) with pm3d notitle\n", rate);
  fprintf(gnuplotFile, "unset multiplot\n");
  return 0;
}
