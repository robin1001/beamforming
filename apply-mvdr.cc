/* Created on 2016-08-29
 * Author: Zhang Binbin
 */

#include <stdio.h>

#include "wav.h"
#include "tdoa.h"
#include "mvdr.h"
#include "vad.h"

int main(int argc, char *argv[]) {

    const char *usage = "Do MVDR beamforming\n"
                        "Usage: apply-mvdr multi_channel_file output_file\n";
    if (argc != 3) {
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
    int num_channel = reader.NumChannel();
    float frame_len = 0.025; // 25 ms
    float frame_shift = 0.01; // 10ms
    int num_point_per_frame = (int)(frame_len * sample_rate);
    int num_point_shift = (int)(frame_shift * sample_rate);

    const float *pcm = reader.Data();
    float *out_pcm = (float *)calloc(sizeof(float), num_sample);
    int *tdoa = (int *)calloc(sizeof(int), num_channel);
   
    Vad vad;
    VadInit(&vad, 1.5e7, 3, 10);

    for (int i = 0; i < num_sample; i += num_point_shift) {
        int frame_size = (i + num_point_per_frame > num_sample) ? 
                            num_sample - i : num_point_per_frame;
        // rearrange channel data
        float *data = (float *)calloc(sizeof(float), frame_size * num_channel);
        for (int j = 0; j < num_channel; j++) {
            for (int k = 0; k < frame_size; k++) {
                data[j * frame_size + k] = pcm[(i + k) * num_channel + j];
            }
        }
        // calc delay
        //GccPhatTdoa(data, num_channel, frame_size, 0, 16, tdoa);
        //for (int j = 0; j < num_channel; j++) {
        //    printf("%d ", tdoa[j]);
        //}
        //printf("\n");
        
        // do vad (is noise or not)
        int is_speech = IsSpeech(&vad, data, frame_size); 

        // do MVDR
        
        free(data);
    }

    // Write outfile
    WavWriter wav_writer(out_pcm, num_sample, 1, sample_rate, 
                         reader.BitsPerSample());
    wav_writer.Write(argv[2]);

    free(out_pcm);
    free(tdoa);
    
    return 0;
}
