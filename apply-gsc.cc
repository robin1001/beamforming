/* Created on 2016-09-08
 * Author: Zhang Binbin
 */

#include <stdio.h>

#include "wav.h"
#include "gsc.h"
#include "parse-option.h"

int main(int argc, char *argv[]) {

    const char *usage = "Do GSC beamforming\n"
                        "Usage: apply-gsc multi_channel_file output_file\n";
    ParseOptions po(usage);
    int num_k = 128; // gsc k width
    po.Register("num-k", &num_k, 
                "context window size for gsc update");
    float alpha = 0.01;
    po.Register("alpha", &alpha,
                "learn rate for gsc param update");

    po.Read(argc, argv);

    if (po.NumArgs() != 2) {
        po.PrintUsage();
        exit(1);
    }
    std::string input_file = po.GetArg(1),
                output_file = po.GetArg(2);

    WavReader reader(input_file.c_str());

    printf("input file %s info: \n"
           "sample_rate %d \n"
           "channels %d \n"
           "bits_per_sample_ %d \n",
           input_file.c_str(),
           reader.SampleRate(), 
           reader.NumChannel(),
           reader.BitsPerSample());

    int sample_rate = reader.SampleRate();
    int num_sample = reader.NumSample();
    int num_channel = reader.NumChannel();


    const float *pcm = reader.Data();
    float *out_pcm = (float *)calloc(sizeof(float), num_sample);

    // For first num_k points, do fixed beamforming
    for (int i = 0; i < num_k; i++) {
        for (int j = 0; j < num_channel; j++) {
            out_pcm[i] += pcm[i * num_channel + j];
        }
        out_pcm[i] /= num_channel;
    }

    Gsc gsc(num_channel, num_k, alpha);
    float *data = (float *)calloc(sizeof(float), num_k * num_channel);
    // For left points, do GSC beamforming
    for (int i = num_k; i < num_sample; i++) {
        // rearrange channel data
        for (int j = 0; j < num_k; j++) {
            for (int k = 0; k < num_channel; k++) {
                data[k * num_k + j] = pcm[(i - num_k + j + 1) * num_channel + k];
            }
        }
        gsc.DoBeamformimg(data, num_k, &out_pcm[i]);
    }

    // Write outfile
    WavWriter wav_writer(out_pcm, num_sample, 1, sample_rate, 
                         reader.BitsPerSample());
    wav_writer.Write(output_file.c_str());

    free(out_pcm);
    free(data);
    return 0;
}

