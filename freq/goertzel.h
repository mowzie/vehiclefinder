#ifndef goertzel_h
#define goertzel_h

#define BUFFERSIZE 512

#define WAILHI   1600
#define PIERCEHI 1640
#define YELPHI   1110
#define WAILLOW  800     //can these two be combined?
#define PIERCELOW 780  //
#define YELPLOW   1250 //500

float goertzel(int TARGET_FREQUENCY,int SAMPLING_RATE, double* data);
int processData(int, int, double* chan1);
#endif
