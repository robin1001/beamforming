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
    float *tdoa = (float *)calloc(sizeof(float), num_channel);
   
    Vad vad;
    VadInit(&vad, 1.5e7, 3, 10);
    Mvdr mvdr(sample_rate, 512, num_channel);

    for (int i = 0; i < num_sample; i += num_point_shift) {
        // last frame
        if (i + num_point_per_frame > num_sample) break; 
        // rearrange channel data
        float *data = (float *)calloc(sizeof(float), 
                                      num_point_per_frame * num_channel);
        for (int j = 0; j < num_channel; j++) {
            for (int k = 0; k < num_point_per_frame; k++) {
                data[j * num_point_per_frame + k] = pcm[(i + k) * num_channel + j];
            }
        }
        // Because gccphat based time-delay is not very precise
        // so here we time-delay is not supported here
        // we suppose the signal arrive sensors at the same time(90)
        
        // do vad (is noise or not)
        bool is_speech = IsSpeech(&vad, data, num_point_per_frame); 

        // do MVDR
        mvdr.DoBeamformimg(data, num_point_per_frame, !is_speech, 
                           tdoa, out_pcm + i);
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



