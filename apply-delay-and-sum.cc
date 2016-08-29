/* Created on 2016-08-16
 * Author: Zhang Binbin
 */

#include <stdio.h>

#include "wav.h"
#include "tdoa.h"
#include "ds.h"


int main(int argc, char *argv[]) {

    const char *usage = "Do delay and sum beamforming\n"
                        "Usage: apply-delay-and-sum multi_channel_file output_file\n";
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

    int tdoa_window = 0.25 * wav_reader.SampleRate(); // 250ms
    int beam_window = 0.25 * wav_reader.SampleRate();
    assert(beam_window <= tdoa_window);
    int margin = 0.0001 * wav_reader.SampleRate(); // margin 16

    int num_channel = wav_reader.NumChannel();
    int num_sample = wav_reader.NumSample();
    const float *pcm = wav_reader.Data();
    float *out_pcm = (float *)calloc(sizeof(float), num_sample);
    int *tdoa = (int *)calloc(sizeof(int), num_channel);

    for (int i = 0; i < num_sample; i += beam_window) {
        int tdoa_window_size = (i + tdoa_window > num_sample) ? 
                            num_sample - i : tdoa_window;
        int beam_window_size = (i + beam_window > num_sample) ? 
                            num_sample - i : beam_window;
        assert(beam_window_size <= tdoa_window_size);
        // rearrange channel data
        float *data = (float *)calloc(sizeof(float), 
                                      tdoa_window_size * num_channel);
        float *beam_data = (float *)calloc(sizeof(float), 
                                      beam_window_size * num_channel);

        for (int j = 0; j < num_channel; j++) {
            for (int k = 0; k < tdoa_window_size; k++) {
                data[j * tdoa_window_size + k] = pcm[(i + k) * num_channel + j];
            }
            for (int k = 0; k < beam_window_size; k++) {
                beam_data[j * beam_window_size + k] = pcm[(i + k) * num_channel + j];
            }
        }
        // calc delay
        int tao = margin < tdoa_window_size / 2 ? margin : tdoa_window_size / 2;
        GccPhatTdoa(data, num_channel, tdoa_window_size, 0, tao, tdoa);
        for (int j = 0; j < num_channel; j++) {
            printf("%d ", tdoa[j]);
        }
        printf("\n");

        DelayAndSum(beam_data, num_channel, beam_window_size, tdoa, out_pcm + i);

        free(data);
        free(beam_data);
    }

    // Write outfile
    WavWriter wav_writer(out_pcm, num_sample, 1,
                         wav_reader.SampleRate(), wav_reader.BitsPerSample());
    wav_writer.Write(argv[2]);

    free(out_pcm);
    free(tdoa);
    
    return 0;
}
