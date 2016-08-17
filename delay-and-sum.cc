/* Created on 2016-08-16
 * Author: Zhang Binbin
 */

#include <stdio.h>

#include "wav.h"
#include "beamforming.h"


int main(int argc, char *argv[]) {

    const char *usage = "Do delay and sum beamforming\n"
                        "Usage: delay-and-sum multi_channel_file output_file\n";
    if (argc != 3) {
        printf(usage);
        exit(-1);
    }


    WavReader wav_reader(argv[1]);

    printf("input file %s info: \n"
            "sample_rate %d \n"
            "channels %d \n"
            "bits_per_sample_ %d \n",
            argv[1],
            wav_reader.SampleRate(), 
            wav_reader.NumChannel(),
            wav_reader.BitsPerSample());

    int tdoa_window = 0.250 * wav_reader.SampleRate(); // 250ms
    int margin = 0.010 * wav_reader.SampleRate(); // margin 10ms

    int num_channel = wav_reader.NumChannel();
    int num_sample = wav_reader.NumSample();
    const float *pcm = wav_reader.Data();
    float *out_pcm = (float *)calloc(sizeof(float), num_sample);
    int *tdoa = (int *)calloc(sizeof(int), num_channel);

    for (int i = 0; i < num_sample; i += tdoa_window) {
        int window_size = (i + tdoa_window > num_sample) ? 
                            num_sample - i : tdoa_window;
        // rearrange channel data
        float *data = (float *)calloc(sizeof(float), window_size * num_channel);
        for (int j = 0; j < num_channel; j++) {
            for (int k = 0; k < window_size; k++) {
                data[j * window_size + k] = pcm[(i + k) * num_channel + j];
            }
        }
        // calc delay
        int tao = margin < window_size / 2 ? margin : window_size / 2;
        calc_tdoa(data, num_channel, window_size, 0, tao, tdoa);
        for (int j = 0; j < num_channel; j++) {
            printf("%d ", tdoa[j]);
        }
        printf("\n");

        delay_and_sum(data, num_channel, window_size, tdoa, out_pcm + i);

        free(data);
    }

    // Write outfile
    WavWriter wav_writer(out_pcm, num_sample, 1,
                         wav_reader.SampleRate(), wav_reader.BitsPerSample());
    wav_writer.Write(argv[2]);

    free(out_pcm);
    free(tdoa);
    
    return 0;
}
