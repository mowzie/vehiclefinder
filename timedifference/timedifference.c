
#include "timedifference.h"
#include "spectrogram.h"
#include "../common/wavheader.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

int main(int argc, char *argv[]) {

    printf("%s\r\n", argv[1]);

    FILE *fp;
	fp = fopen(argv[1], "rb");

	if (!fp) {

        printf("invalid file\r\n");
        system("pause");
        return EXIT_FAILURE;
	}

    struct WaveHeader *wav = (WaveHeader*)malloc(sizeof(struct WaveHeader));
    readHeader(fp, wav);
    printHeader(wav);
    frames = wav->chunk_size / (wav->block_align * 512);

	in = (float*) calloc(buckets, sizeof(float));
	out = (float*) calloc(buckets, sizeof(float));
	p = fftwf_plan_r2r_1d(buckets, in, out, FFTW_REDFT00, FFTW_ESTIMATE);

	spectro_channel0 = (float **) malloc (frames * sizeof(float*));

	int i;
	for (i = 0; i < frames; i++) {

        spectro_channel0[i] = (float*) malloc(buckets * sizeof(float));
	}

    for (i = 0; i < frames; i++) {
        readDataIntoBuffer(in, fp);
        fftwf_execute(p); /* repeat as needed */

        memcpy(spectro_channel0[i], out, sizeof(float) * buckets);
    }
    fclose(fp);
    fftwf_destroy_plan(p);

    float min = FLT_MAX;
    float max = FLT_MIN;
    int j;
    for (i = 0; i < frames; i++) {
        for (j = 0; j < buckets; j++) {

            if (spectro_channel0[i][j] < min)
                min = spectro_channel0[i][j];
            if (spectro_channel0[i][j] > max)
                max = spectro_channel0[i][j];
        }
    }
    char* filename = strcat(argv[1], "_wav.ppm");
    FILE *f = fopen(filename, "wb");
    fprintf(f, "P6\n%i %i 255\n", frames, buckets);
    for (i = 0; i < buckets; i++)
        for (j = 0; j < frames; j++) {

            float brightness = log10(fabs(spectro_channel0[j][buckets - i])) / log10(fabs(max));
            if (brightness > 1)
                brightness = 1;
            if (brightness < 0)
                brightness = 0;
            fputc((char)(brightness * 255), f);   // 0 .. 255
            fputc((char)(brightness * 255), f); // 0 .. 255
            fputc((char)(brightness * 255), f);  // 0 .. 255
        }
    fclose(f);
    printf("Created \"%s\"\r\n", filename);

	fflush(stdout);
    free(wav);
	free(in);
	free(out);

	for (i = 0; i < frames; i++) {

        free(spectro_channel0[i]);
	}
	free(spectro_channel0);
    fftwf_cleanup();
	system("pause");
	return EXIT_SUCCESS;
}


