#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../common/wavheader.h"

void writeHeader(FILE *fOut, struct WaveHeader *header);
void showUsage();

int main(int argc, char *argv[]){
  int i;
  char wav1FileName[300];
  char wav2FileName[300];
  char wav3FileName[300];
  char wav4FileName[300];
  struct WaveHeader *wav1;
  struct WaveHeader *wav2;
  struct WaveHeader *wav3;
  struct WaveHeader *wav4;
  struct WaveHeader *wavOut;
  int channelCount = 0;
  unsigned int totalSamples;

  FILE *fWav1;
  FILE *fWav2;
  FILE *fWav3;
  FILE *fWav4;
  FILE *fOut;

  printf("Total args %d\n", argc);
  if (argc != 9) {
    showUsage();
    return 1;
  }

  for (i = 1; i < (argc); i++) {
  printf("arg: %s\n", argv[i]);

    if (strcasecmp("-wav1", argv[i])==0) {
      printf("wav1\n");
      //require a filename to go along with the -file
      //check -file isn't the last arg, otherwise it throws a sigsegv
      if (i+1 == argc) {
        fprintf(stderr,"\n**ERROR: No filename listed with \"-wav1\"\n");
        return 1;
      }
      strncpy(wav1FileName, argv[++i],300);
      fWav1 = fopen(wav1FileName, "rb");
      if (!fWav1) {
        fprintf(stderr,"\n**ERROR: Could not open file \"%s\"\n", wav1FileName);
        return 1;
      }
	  channelCount++;
      continue;
    }

	  if (strcasecmp("-wav2", argv[i])==0) {
        printf("wav2\n");
      //require a filename to go along with the -file
      //check -file isn't the last arg, otherwise it throws a sigsegv
      if (i+1 == argc) {
        fprintf(stderr,"\n**ERROR: No filename listed with \"-wav2\"\n");
        return 1;
      }
      strncpy(wav2FileName, argv[++i],300);
      fWav2 = fopen(wav2FileName, "rb");
      if (!fWav2) {
        fprintf(stderr,"\n**ERROR: Could not open file \"%s\"\n", wav2FileName);
        return 1;
      }
	    channelCount++;
      continue;
    }
    if (strcasecmp("-wav3", argv[i])==0) {
      //require a filename to go along with the -file
      //check -file isn't the last arg, otherwise it throws a sigsegv
      if (i+1 == argc) {
        fprintf(stderr,"\n**ERROR: No filename listed with \"-wav3\"\n");
        return 1;
      }
      strncpy(wav3FileName, argv[++i],300);
      fWav3 = fopen(wav3FileName, "rb");
      if (!fWav3) {
        fprintf(stderr,"\n**ERROR: Could not open file \"%s\"\n", wav3FileName);
        return 1;
      }
	    channelCount++;
      continue;
    }
    if (strcasecmp("-wav4", argv[i])==0) {
      //require a filename to go along with the -file
      //check -file isn't the last arg, otherwise it throws a sigsegv
      if (i+1 == argc) {
        fprintf(stderr,"\n**ERROR: No filename listed with \"-wav4\"\n");
        return 1;
      }
      strncpy(wav4FileName, argv[++i],300);
      fWav4 = fopen(wav4FileName, "rb");
      if (!fWav4) {
        fprintf(stderr,"\n**ERROR: Could not open file \"%s\"\n", wav4FileName);
        return 1;
      }
	   channelCount++;
      continue;
    }
  }

  //it's a little hacky, but this checks if we have a file
  if(fWav1==NULL) {
    showUsage();
    return 1;
  }
  if(fWav2==NULL) {
    showUsage();
    return 1;
  }
  if(fWav3==NULL) {
    showUsage();
    return 1;
  }
  if(fWav4==NULL) {
    showUsage();
    return 1;
  }

	wav1 = malloc(sizeof(struct WaveHeader));
  strncpy(wav1->wavName,wav1FileName ,300);
  if (readHeader(fWav1, wav1) != 0) {
    fprintf(stderr, "Error reading (%s), check if it is a valid wav file\n",wav1FileName);
    return 1;
  }
  wav2 = malloc(sizeof(struct WaveHeader));
  strncpy(wav2->wavName,wav2FileName ,300);
  if (readHeader(fWav2, wav2) != 0) {
    fprintf(stderr, "Error reading (%s), check if it is a valid wav file\n",wav2FileName);
    return 1;
  }
  wav3 = malloc(sizeof(struct WaveHeader));
  strncpy(wav3->wavName,wav3FileName ,300);
  if (readHeader(fWav3, wav3) != 0) {
    fprintf(stderr, "Error reading (%s), check if it is a valid wav file\n",wav3FileName);
    return 1;
  }
  wav4 = malloc(sizeof(struct WaveHeader));
  strncpy(wav4->wavName,wav4FileName ,300);
  if (readHeader(fWav4, wav4) != 0) {
    fprintf(stderr, "Error reading (%s), check if it is a valid wav file\n",wav4FileName);
    return 1;
  }

  readAllData(fWav1,wav1);
  readAllData(fWav2,wav2);
  readAllData(fWav3,wav3);
  readAllData(fWav4,wav4);

  wavOut = malloc(sizeof(struct WaveHeader));
  fOut = fopen("out.wav", "wb");
  if (!fOut) {
    printf("FAIL!\n");
    return 1;
  }

  wavOut->chunkId[0] = 'R';
  wavOut->chunkId[1] = 'I';
  wavOut->chunkId[2] = 'F';
  wavOut->chunkId[3] = 'F';
	wavOut->chunkSize=36 + (channelCount * wav1->datachunkSize);
	wavOut->wavID[0]   = 'W';
  wavOut->wavID[1] = 'A';
  wavOut->wavID[2] = 'V';
  wavOut->wavID[3] = 'E';
	wavOut->fmtID[0] = 'f';
  wavOut->fmtID[1] = 'm';
  wavOut->fmtID[2] = 't';
  wavOut->fmtID[3] = ' ';
	wavOut->fmtChunkSize= wav1->fmtChunkSize;
	wavOut->wFormatTag=wav1->wFormatTag;
	wavOut->nChannels = channelCount;
	wavOut->sampleRate=wav1->sampleRate;
	wavOut->byteRate = wav1->byteRate;
	wavOut->blockAlign = wav1->blockAlign;
	wavOut->bps = wav1->bps;
	wavOut->datachunkId[0] = 'd';
  wavOut->datachunkId[1] = 'a';
  wavOut->datachunkId[2] = 't';
  wavOut->datachunkId[3] = 'a';
	wavOut->datachunkSize=channelCount * wav1->datachunkSize;

  printHeader(wavOut);
  totalSamples = wav1->datachunkSize / (wav1->nChannels * (wav1->bps / 8));
  writeHeader(fOut, wavOut);

  for (i = 0; i < totalSamples; i++) {
    fwrite(&wav1->chan1[i],1,2,fOut);
    fwrite(&wav2->chan1[i],1,2,fOut);
    fwrite(&wav3->chan1[i],1,2,fOut);
    fwrite(&wav4->chan1[i],1,2,fOut);
  }

  freeChannelMemory(wav1);
  freeChannelMemory(wav2);
  freeChannelMemory(wav3);
  freeChannelMemory(wav4);
	fclose(fWav1);
  fclose(fWav2);
  fclose(fWav3);
  fclose(fWav4);
  fclose(fOut);
  free(wav1);
  free(wav2);
  free(wav3);
  free(wav4);
  free(wavOut);

  return 0;
}


void showUsage() {
  printf("Usage: ./wavCombine -wav1 <wav> -wav2 <wav> -wav3 <wav> -wav4 <wav>\n");
  printf("       Combines 4 single channel wav files into 1 4-channel wav file\n");
  printf("\n");
}


void writeHeader(FILE *fOut, struct WaveHeader *header) {
  fwrite(&header->chunkId, sizeof(char), 4, fOut); //read in first four bytes
  fwrite(&header->chunkSize, sizeof(int), 1, fOut); //read in 32bit size value
  fwrite(&header->wavID, sizeof(char), 4, fOut);
  fwrite(&header->fmtID, sizeof(char), 4, fOut);
  fwrite(&header->fmtChunkSize, sizeof(int), 1, fOut);
  fwrite(&header->wFormatTag, sizeof(unsigned short int), 1, fOut);
  fwrite(&header->nChannels, sizeof(short int), 1, fOut);
  fwrite(&header->sampleRate, sizeof(int), 1, fOut);
  fwrite(&header->byteRate, sizeof(int), 1, fOut);
  fwrite(&header->blockAlign, sizeof(short int), 1, fOut);
  fwrite(&header->bps, sizeof(short int), 1, fOut);
  fwrite(&header->datachunkId, sizeof(char),4,fOut);
  fwrite(&header->datachunkSize, sizeof(unsigned int),1,fOut);
  return;
}
