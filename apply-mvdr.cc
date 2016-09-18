/* Created on 2016-08-29
 * Author: Zhang Binbin
 */

#include <stdio.h>

#include "wav.h"
#include "tdoa.h"
#include "mvdr.h"
#include "vad.h"
#include "parse-option.h"

int main(int argc, char *argv[]) {

    const char *usage = "Do MVDR beamforming\n"
                        "Usage: apply-mvdr multi_channel_file output_file\n";
    ParseOptions po(usage);

    float frame_len = 0.025; // 25 ms
    po.Register("frame-len", &frame_len, "frame length for mvdr");
    float frame_shift = 0.01; // 10ms
    po.Register("frame-shift", &frame_shift, "frame shift for mvdr");
    int fft_point = 512;
    po.Register("fft-point", &fft_point, 
                "num fft point for spectrum calculation, must be 2^n"
                "and must greater than frame_len * sample_rate");
    float energy_thresh = 1.5e-7;
    po.Register("energy-thresh", &energy_thresh, 
                "energy threshold for energy based vad");
    int sil_to_speech_trigger = 3;
    po.Register("sil-to-speech-trigger", &sil_to_speech_trigger,
                "num frames for silence to speech trigger");
    int speech_to_sil_trigger = 10;
    po.Register("speech-to-sil-trigger", &speech_to_sil_trigger,
                "num frames for speech to silence trigger");
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
    int num_point_per_frame = (int)(frame_len * sample_rate);
    int num_point_shift = (int)(frame_shift * sample_rate);

    const float *pcm = reader.Data();
    float *out_pcm = (float *)calloc(sizeof(float), num_sample);
    float *tdoa = (float *)calloc(sizeof(float), num_channel);
   
    Vad vad;
    VadInit(&vad, energy_thresh, sil_to_speech_trigger, speech_to_sil_trigger);
    Mvdr mvdr(sample_rate, fft_point, num_channel);

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
    wav_writer.Write(output_file.c_str());

    free(out_pcm);
    free(tdoa);
    
    return 0;
}



