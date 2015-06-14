#include <math.h>
#include "correlation.h"

float getTimeDomainAC(double* X, int N, int init) {
  static double previous = 0.0;
  int state = 0;
  int i = 0;
  int t = 0;
  double m = 0.0;
  double mAB = 0.0;
  double s = 0.0;
  double max = 0.0;
  double denom = 0.0;
  double rdelay = 0.0;
  double maxDelay = 65;
  double tDelta = 0.0;
  double rdelay_previous = 0.0;

  for (i = 0; i < N; i++) {
    m += X[i];
  }
  m /= N;

  for (i = 0; i < N-1; i++) {
    s += ((X[i] - m));
  }
  denom = s*s;

  for (t = init; (t < maxDelay) && (state != 2); t++) {
    mAB = 0;
    for (i = 0; i < N-1; i++) {
      rdelay_previous = rdelay;
      if ((i + t) < 0 || (i + t) >= N)
        continue;
      else {
        mAB += (X[i] - m) * (X[i + t] - m);
      }
    }
    rdelay = mAB / denom;
    if ((state == 0) && (rdelay > 0) && (rdelay > rdelay_previous)) {
      state = 1;
    }
    else if ((state ==1) && (rdelay < rdelay_previous)) {
      state = 0;
      if (rdelay > max) {
        max = rdelay;
        tDelta = t -1;
      }
    }
  //  printf("%f %f\n", 44100.0/t, rdelay);
  }
//  printf("\n");
  if (tDelta == init) tDelta = previous;
  previous = tDelta;

  return tDelta;
}

float getTimeDomainCC(double* sigA, double* sigB, int N) {
  double mA, mB,mAB,sA,sB;
  double max = 0;
  double denom = 0;
  double rdelay = 0;
  double delay = 0;
  double maxDelay = 200;
  double tDelta = 0;
  int j;
  int foo = 0;

  for (j = 0; j < N; j++) {
    mA += sigA[j];
    mB += sigB[j];
  }
  mA /= N;
  mB /= N;
  sA = 0;
  sB = 0;
for (j = 0; j < N; j++) {
    sA += (sigA[j] - mA) * (sigA[j] - mA);
    sB += (sigB[j] - mB) * (sigB[j] - mB);
  }
  denom = sqrtf(sA * sB);

  for (delay = -maxDelay; delay < maxDelay; delay++) {
    mAB = 0;
    for (j = 0; j < N; j++) {
      foo = j + delay;
      if (foo < 0 || foo >= N)
        continue;
      else {
        mAB += (sigA[j] - mA) * (sigB[foo] - mB);
      }
    }

    rdelay = mAB / denom;

    if ((rdelay) > (max)) {
      max = rdelay;
      tDelta = delay;
    }
  }
  return tDelta / 44100.0;
}
