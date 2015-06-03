#include <math.h>
#include <stdio.h>
#define BUFFERSIZE 512

#define WAILHI   1600
#define PIERCEHI 1640
#define YELPHI   1110
#define WAILLOW  800     //can these two be combined?
#define PIERCELOW 780  //
#define YELPLOW   1250 //500

//#define YELPLOW 610 //police.wav


float goertzel(int TARGET_FREQUENCY,int SAMPLING_RATE, short int* data);
int processData(int, int, short int* chan1);

float goertzel(int TARGET_FREQUENCY,int SAMPLING_RATE, short int* data)
{
  int   k,i;
  float floatnumSamples;
  float omega = 0;
  float sine = 0;
  float cosine = 0;
  float coeff = 0;
  float q0 = 0;
  float q1 = 0;
  float q2 = 0;
  float real = 0;
  float imag = 0;
  float foo = 0.0;

  floatnumSamples = (float) BUFFERSIZE;
  k = (int) ( ((0.5 + floatnumSamples * TARGET_FREQUENCY) / SAMPLING_RATE));
  omega = (2.0 * M_PI *k) / floatnumSamples;
  sine = sin(omega);    //imag
  cosine = cos(omega);
  coeff = 2.0 * cosine; //real
  q0=0;
  q1=0;
  q2=0;

  for(i=1; i<BUFFERSIZE; i++) {
    foo  = data[i]*(0.54 - 0.46 * cos(2 * M_PI * i / BUFFERSIZE));
    q0 = foo + coeff * q1 - q2;
    q2 = q1;
    q1 = q0;
  }

  real = (q1 - q2 * cosine) / (BUFFERSIZE / 2.0);
  imag = (q2 * sine) / (BUFFERSIZE / 2.0);
  return sqrtf(real*real + imag*imag);
}


int processData(int freq, int sample, short int *foo) {
  //int test = 0;

  //static int i = 0;
 // int j = 0;
  float first = 0.0;
  float second = 0.0;
  float power = 0.0;
  //int count = 0;
 // int last = 0;
 // int wail = 0;
 // int wailC = 0;
 static int wailC = 0;
 static int lastWail = 0;
  static int pierceC = 0;
  static int yelpC = 0;
  static int lastPierce = 0;
  static int lastYelp = 0;

  int hilo = 0;
  //int hiloC = 0;
  //short int foo [BUFFERSIZE];

//   printf("%d clap %f \n", sample, power);

//  power = goertzel(YELPLOW,freq, foo);

/*
printf("%d 500 %f \n", sample, goertzel(500,freq, foo));
printf("%d 505 %f \n", sample, goertzel(505,freq, foo));
printf("%d 510 %f \n", sample, goertzel(510,freq, foo));
printf("%d 515 %f \n", sample, goertzel(515,freq, foo));
printf("%d 520 %f \n", sample, goertzel(520,freq, foo));
printf("%d 525 %f \n", sample, goertzel(525,freq, foo));
printf("%d 530 %f \n", sample, goertzel(530,freq, foo));
printf("%d 535 %f \n", sample, goertzel(535,freq, foo));
printf("%d 540 %f \n", sample, goertzel(540,freq, foo));
printf("%d 545 %f \n", sample, goertzel(545,freq, foo));
printf("%d 550 %f \n", sample, goertzel(550,freq, foo));
printf("%d 555 %f \n", sample, goertzel(555,freq, foo));
printf("%d 560 %f \n", sample, goertzel(560,freq, foo));
printf("%d 565 %f \n", sample, goertzel(565,freq, foo));
printf("%d 570 %f \n",sample, goertzel(570,freq, foo));
printf("%d 575 %f \n",sample, goertzel(575,freq, foo));
printf("%d 580 %f \n", sample, goertzel(580,freq, foo));
printf("%d 585 %f \n", sample, goertzel(585,freq, foo));
printf("%d 590 %f \n", sample, goertzel(590,freq, foo));
printf("%d 595 %f \n", sample, goertzel(595,freq, foo));


*/
/*
    printf("%d 600 %f \n", sample, goertzel(600,freq, foo));
    printf("%d 605 %f \n", sample, goertzel(605,freq, foo));
    printf("%d 610 %f \n", sample, goertzel(610,freq, foo));
    printf("%d 615 %f \n", sample, goertzel(615,freq, foo));
    printf("%d 620 %f \n", sample, goertzel(620,freq, foo));
    printf("%d 625 %f \n", sample, goertzel(625,freq, foo));
    printf("%d 630 %f \n", sample, goertzel(630,freq, foo));
    printf("%d 635 %f \n", sample, goertzel(635,freq, foo));
    printf("%d 640 %f \n", sample, goertzel(640,freq, foo));
    printf("%d 645 %f \n", sample, goertzel(645,freq, foo));
    printf("%d 650 %f \n", sample, goertzel(650,freq, foo));
    printf("%d 655 %f \n", sample, goertzel(655,freq, foo));
    printf("%d 660 %f \n", sample, goertzel(660,freq, foo));
    printf("%d 665 %f \n", sample, goertzel(665,freq, foo));
    printf("%d 670 %f \n",sample, goertzel(670,freq, foo));
    printf("%d 675 %f \n",sample, goertzel(675,freq, foo));
    printf("%d 680 %f \n", sample, goertzel(680,freq, foo));
    printf("%d 685 %f \n", sample, goertzel(685,freq, foo));
    printf("%d 690 %f \n", sample, goertzel(690,freq, foo));
    printf("%d 695 %f \n", sample, goertzel(695,freq, foo));
    printf("%d 700 %f \n", sample, goertzel(700,freq, foo));
    printf("%d 705 %f \n", sample, goertzel(705,freq, foo));
    printf("%d 710 %f \n", sample, goertzel(710,freq, foo));
    printf("%d 715 %f \n", sample, goertzel(715,freq, foo));
    printf("%d 720 %f \n", sample, goertzel(720,freq, foo));
    printf("%d 725 %f \n", sample, goertzel(725,freq, foo));
    printf("%d 730 %f \n", sample, goertzel(730,freq, foo));
    printf("%d 735 %f \n", sample, goertzel(735,freq, foo));
    printf("%d 740 %f \n", sample, goertzel(740,freq, foo));
    printf("%d 745 %f \n", sample, goertzel(745,freq, foo));
    printf("%d 750 %f \n", sample, goertzel(750,freq, foo));
    printf("%d 755 %f \n", sample, goertzel(755,freq, foo));
    printf("%d 760 %f \n", sample, goertzel(760,freq, foo));
    printf("%d 765 %f \n", sample, goertzel(765,freq, foo));
    printf("%d 770 %f \n", sample, goertzel(770,freq, foo));
    printf("%d 775 %f \n", sample, goertzel(775,freq, foo));
    printf("%d 780 %f \n", sample, goertzel(780,freq, foo));
    printf("%d 785 %f \n", sample, goertzel(785,freq, foo));
    printf("%d 790 %f \n", sample, goertzel(790,freq, foo));
    printf("%d 795 %f \n", sample, goertzel(795,freq, foo));
    printf("%d 800 %f \n", sample, goertzel(800,freq, foo));
    printf("%d 805 %f \n", sample, goertzel(805,freq, foo));
    printf("%d 810 %f \n", sample, goertzel(810,freq, foo));
    printf("%d 815 %f \n", sample, goertzel(815,freq, foo));
    printf("%d 820 %f \n", sample, goertzel(820,freq, foo));
    printf("%d 825 %f \n", sample, goertzel(825,freq, foo));
    printf("%d 830 %f \n", sample, goertzel(830,freq, foo));
    printf("%d 835 %f \n", sample, goertzel(835,freq, foo));
    printf("%d 840 %f \n", sample, goertzel(840,freq, foo));
    printf("%d 845 %f \n", sample, goertzel(845,freq, foo));
    printf("%d 850 %f \n", sample, goertzel(850,freq, foo));
    printf("%d 855 %f \n", sample, goertzel(855,freq, foo));
    printf("%d 860 %f \n", sample, goertzel(860,freq, foo));
    printf("%d 865 %f \n", sample, goertzel(865,freq, foo));
    printf("%d 870 %f \n" ,sample, goertzel(870,freq, foo));
    printf("%d 875 %f \n" ,sample, goertzel(875,freq, foo));
    printf("%d 880 %f \n", sample, goertzel(880,freq, foo));
    printf("%d 885 %f \n", sample, goertzel(885,freq, foo));
    printf("%d 890 %f \n", sample, goertzel(890,freq, foo));
    printf("%d 895 %f \n", sample, goertzel(895,freq, foo));
    printf("%d 900 %f \n", sample, goertzel(900,freq, foo));
    printf("%d 905 %f \n", sample, goertzel(905,freq, foo));
    printf("%d 910 %f \n", sample, goertzel(910,freq, foo));
    printf("%d 915 %f \n", sample, goertzel(915,freq, foo));
    printf("%d 920 %f \n", sample, goertzel(920,freq, foo));
    printf("%d 925 %f \n", sample, goertzel(925,freq, foo));
    printf("%d 930 %f \n", sample, goertzel(930,freq, foo));
    printf("%d 935 %f \n", sample, goertzel(935,freq, foo));
    printf("%d 940 %f \n", sample, goertzel(940,freq, foo));
    printf("%d 945 %f \n", sample, goertzel(945,freq, foo));
    printf("%d 950 %f \n", sample, goertzel(950,freq, foo));
    printf("%d 955 %f \n", sample, goertzel(955,freq, foo));
    printf("%d 960 %f \n", sample, goertzel(960,freq, foo));
    printf("%d 965 %f \n", sample, goertzel(965,freq, foo));
    printf("%d 970 %f \n", sample, goertzel(970,freq, foo));
    printf("%d 975 %f \n", sample, goertzel(975,freq, foo));
    printf("%d 980 %f \n", sample, goertzel(980,freq, foo));
    printf("%d 985 %f \n", sample, goertzel(985,freq, foo));
    printf("%d 990 %f \n", sample, goertzel(990,freq, foo));

    /*
    printf("%d 995 %f \n", sample, goertzel(995,freq, foo));
    printf("%d 1000 %f \n", sample, goertzel(1000,freq, foo));


printf("%d 1105 %f \n", sample, goertzel(1105,freq, foo));
printf("%d 1110 %f \n", sample, goertzel(1110,freq, foo));
printf("%d 1115 %f \n", sample, goertzel(1115,freq, foo));
printf("%d 1120 %f \n", sample, goertzel(1120,freq, foo));
printf("%d 1125 %f \n", sample, goertzel(1125,freq, foo));
printf("%d 1130 %f \n", sample, goertzel(1130,freq, foo));
printf("%d 1135 %f \n", sample, goertzel(1135,freq, foo));
printf("%d 1140 %f \n", sample, goertzel(1140,freq, foo));
printf("%d 1145 %f \n", sample, goertzel(1145,freq, foo));
printf("%d 1150 %f \n", sample, goertzel(1150,freq, foo));
printf("%d 1155 %f \n", sample, goertzel(1155,freq, foo));
printf("%d 1160 %f \n", sample, goertzel(1160,freq, foo));
printf("%d 1165 %f \n", sample, goertzel(1165,freq, foo));
printf("%d 1170 %f \n", sample, goertzel(1170,freq, foo));
printf("%d 1175 %f \n", sample, goertzel(1175,freq, foo));
printf("%d 1180 %f \n", sample, goertzel(1180,freq, foo));
printf("%d 1185 %f \n", sample, goertzel(1185,freq, foo));
printf("%d 1190 %f \n", sample, goertzel(1190,freq, foo));
printf("%d 1195 %f \n", sample, goertzel(1195,freq, foo));
printf("%d 1200 %f \n", sample, goertzel(1200,freq, foo));




printf("%d 1505 %f \n", sample, goertzel(1505,freq, foo));
printf("%d 1510 %f \n", sample, goertzel(1510,freq, foo));
printf("%d 1515 %f \n", sample, goertzel(1515,freq, foo));
printf("%d 1520 %f \n", sample, goertzel(1520,freq, foo));
printf("%d 1525 %f \n", sample, goertzel(1525,freq, foo));
printf("%d 1530 %f \n", sample, goertzel(1530,freq, foo));
printf("%d 1535 %f \n", sample, goertzel(1535,freq, foo));
printf("%d 1540 %f \n", sample, goertzel(1540,freq, foo));
printf("%d 1545 %f \n", sample, goertzel(1545,freq, foo));
printf("%d 1550 %f \n", sample, goertzel(1550,freq, foo));
printf("%d 1555 %f \n", sample, goertzel(1555,freq, foo));
printf("%d 1560 %f \n", sample, goertzel(1560,freq, foo));
printf("%d 1565 %f \n", sample, goertzel(1565,freq, foo));
printf("%d 1570 %f \n", sample, goertzel(1570,freq, foo));
printf("%d 1575 %f \n", sample, goertzel(1575,freq, foo));
printf("%d 1580 %f \n", sample, goertzel(1580,freq, foo));
printf("%d 1585 %f \n", sample, goertzel(1585,freq, foo));
printf("%d 1590 %f \n", sample, goertzel(1590,freq, foo));
printf("%d 1595 %f \n", sample, goertzel(1595,freq, foo));
*/
/*
printf("%d 1600 %f \n", sample, goertzel(1600,freq, foo));
printf("%d 1605 %f \n", sample, goertzel(1605,freq, foo));
printf("%d 1610 %f \n", sample, goertzel(1610,freq, foo));
printf("%d 1615 %f \n", sample, goertzel(1615,freq, foo));
printf("%d 1620 %f \n", sample, goertzel(1620,freq, foo));
printf("%d 1625 %f \n", sample, goertzel(1625,freq, foo));
printf("%d 1630 %f \n", sample, goertzel(1630,freq, foo));
printf("%d 1635 %f \n", sample, goertzel(1635,freq, foo));
printf("%d 1640 %f \n", sample, goertzel(1640,freq, foo));
printf("%d 1645 %f \n", sample, goertzel(1645,freq, foo));
printf("%d 1650 %f \n", sample, goertzel(1650,freq, foo));
printf("%d 1655 %f \n", sample, goertzel(1655,freq, foo));
printf("%d 1660 %f \n", sample, goertzel(1660,freq, foo));
printf("%d 1665 %f \n", sample, goertzel(1665,freq, foo));
printf("%d 1670 %f \n",sample, goertzel(1670,freq, foo));
printf("%d 1675 %f \n",sample, goertzel(1675,freq, foo));
printf("%d 1680 %f \n", sample, goertzel(1680,freq, foo));
printf("%d 1685 %f \n", sample, goertzel(1685,freq, foo));
printf("%d 1690 %f \n", sample, goertzel(1690,freq, foo));
printf("%d 1695 %f \n", sample, goertzel(1695,freq, foo));
printf("%d 1700 %f \n", sample, goertzel(1700,freq, foo));
printf("%d 1705 %f \n", sample, goertzel(1705,freq, foo));
printf("%d 1710 %f \n", sample, goertzel(1710,freq, foo));
printf("%d 1715 %f \n", sample, goertzel(1715,freq, foo));
printf("%d 1720 %f \n", sample, goertzel(1720,freq, foo));
printf("%d 1725 %f \n", sample, goertzel(1725,freq, foo));
printf("%d 1730 %f \n", sample, goertzel(1730,freq, foo));
printf("%d 1735 %f \n", sample, goertzel(1735,freq, foo));
printf("%d 1740 %f \n", sample, goertzel(1740,freq, foo));
printf("%d 1745 %f \n", sample, goertzel(1745,freq, foo));
printf("%d 1750 %f \n", sample, goertzel(1750,freq, foo));
printf("%d 1755 %f \n", sample, goertzel(1755,freq, foo));
printf("%d 1760 %f \n", sample, goertzel(1760,freq, foo));
printf("%d 1765 %f \n", sample, goertzel(1765,freq, foo));
printf("%d 1770 %f \n", sample, goertzel(1770,freq, foo));
printf("%d 1775 %f \n", sample, goertzel(1775,freq, foo));
printf("%d 1780 %f \n", sample, goertzel(1780,freq, foo));
printf("%d 1785 %f \n", sample, goertzel(1785,freq, foo));
printf("%d 1790 %f \n", sample, goertzel(1790,freq, foo));
printf("%d 1795 %f \n", sample, goertzel(1795,freq, foo));
printf("%d 1800 %f \n", sample, goertzel(1800,freq, foo));
/*
*/


//960
//    goertzel(200,freq, foo);
//    goertzel(960,freq, foo);
//  goertzel(960,freq, foo);
//  goertzel(960,freq, foo);
/*
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
        power = goertzel(600,freq, foo);
        //if (power > 1000) {
          //printf("%d %f low \n", sample, power);
          //hilo = 1;
        //}
      }
      //else {
        power = goertzel(1500,freq, foo);
        //if (power > 800) {
        //printf("%d %f high \n", i+BUFFERSIZE, power);
       // count++;
       // last = 0;
       // hilo = 0;
      //}
      //else
      //    last=last + BUFFERSIZE;
      //}
    //  if (last > 2560) {
    //    last = 0;
    //  }
    //  if (count > 15) {
      //printf("High Low Siren\n");
      //return;
     // }
*/



power = goertzel(WAILLOW,freq, foo);
first = power;
if (power > 1050) {
  if ((sample - lastWail) > 60000) {
    wailC++;
    lastWail = sample;
    //printf("%d\tlow\t%f\t%d\n", sample,power,wailC);
  }

}
power = goertzel(WAILHI,freq, foo);
second = power;
if (power > 6000) {
  if ((sample-lastWail) > 80000) {
    wailC++;
    lastWail = sample;
    //printf("%d\thi\t%f\t%d\n", sample,power,wailC);
  }

}
if ((sample - lastWail) > 200000)
  wailC = 0;
if (wailC >1500) {
  printf("*********WAIL*********");
  return 1;
}


//////////////////////////////////////
power = goertzel(YELPLOW,freq, foo);
first = power;
if (power > 1200) {
  printf("%d \t %f", sample, power);
  if ((sample - lastYelp) > 6000) {
    yelpC++;
    lastYelp = sample;

  }
} else {
  printf("%d \t 0", sample);
}
power = goertzel(YELPHI,freq, foo);
second = power;
if (power > 1200) {
  //printf("%d\thi\t%f\t%d yelp\n", sample,power,yelpC);
  printf("\t %f \n", power);
  if ((sample-lastYelp) > 4000) {
    yelpC++;
    lastYelp = sample;

  }
} else {
  printf("\t 0\n");
}
if ((sample - lastYelp) > 10000)
yelpC = 0;
if (yelpC >400000) {
//  printf("*********YELP*********\n");
  yelpC = 0;
  return 1;
}
///////////////////////////////////
//
//Search for "Yelp"  (oscilations between 700hz -> 1700 hz)
//
///////////////////////////////////

//average yelp cycle is between 700-770 (based on sample size)

      power = goertzel(PIERCELOW,freq, foo);
//      printf("%d\tlow\t %f \t%d pierce\n", sample, power,pierceC);
      if (power > 300) {
        if ((sample - lastPierce) > 2000) {
          lastPierce = sample;
          pierceC++;

        }
    //    printf("%d\tlow\t %f \t%d pierce\n", sample, power,pierceC);

      }
      power = goertzel(PIERCEHI,freq, foo);

      if (power > 250) {
        if ((sample-lastPierce) > 2000) {
          lastPierce = sample;
          pierceC++;
        }
      //  printf("%d\thi\t%f\t%d pierce\n", sample,power,pierceC);

      }
      if ((sample - lastPierce) > 3000)
        pierceC = 0;
      if (pierceC >4) {
      //  printf("*********PIERCE*********\n");
        pierceC = 0;
        return 1;
      }

//////////////////////////////////////
//End Wail search
//////////////////////////////////////


  return 0;
}
