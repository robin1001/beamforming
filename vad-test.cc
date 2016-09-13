/* Created on 2016-08-24
 * Author: Zhang Binbin
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "wav.h"
#include "vad.h"


int main(int argc, char *argv[]) {
    const char *usage = "Do energy vad for input wav file\n"
                        "Usage: vad-test wav_in_file\n";
    if (argc != 2) {
        printf(usage);
        exit(-1);
    }

    WavReader reader(argv[1]);

    printf("input file %s info: \n"
           "sample_rate %d \n"
           "channels %d \n"
           "bits_per_sample_ %d \n",
           argv[1],
           reader.SampleRate(), 
           reader.NumChannel(),
           reader.BitsPerSample());
    
    int sample_rate = reader.SampleRate();
    int num_sample = reader.NumSample();
    float frame_len = 0.025; // 25 ms
    float frame_shift = 0.01; // 10ms
    int num_point_per_frame = (int)(frame_len * sample_rate);
    int num_point_shift = (int)(frame_shift * sample_rate);
   
    float *data = (float *)calloc(sizeof(float), num_sample);
    // Copy first channel
    for (int i = 0; i < num_sample; i++) {
        data[i] = reader.Data()[i * reader.NumChannel()];
    }

    Vad vad;
    VadInit(&vad, 1.5e7, 3, 10);

    for (int i = 0; i < num_sample; i += num_point_shift) {
        // last frame 
        if (i + num_point_per_frame > num_sample) break;
        int tags = IsSpeech(&vad, data+i, num_point_per_frame) ? 1 : 0;
        printf("%f %d \n", float(i) / sample_rate, tags);
    }

    free(data);
    return 0;
}


