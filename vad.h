/* Created on 2016-08-24
 * Author: Zhang Binbin
 */

#ifndef VAD_H_
#define VAD_H_

#include <assert.h>

typedef enum {
    kSpeech,
    kSilence
} VadState;


typedef struct {
    float energy_thresh;        
    int silence_to_speech_thresh;
    int speech_to_sil_thresh;
    int silence_frame_count;
    int speech_frame_count;
    int frame_count;
    VadState state;
} Vad;


void VadInit(Vad *vad, float energy_thresh, int speech_thresh, int sil_thresh) {
    vad->energy_thresh = energy_thresh;
    vad->silence_to_speech_thresh = speech_thresh;
    vad->speech_to_sil_thresh = sil_thresh;
    vad->silence_frame_count = 0;
    vad->speech_frame_count = 0;
    vad->frame_count = 0;
    vad->state = kSilence;
}

// return 1 if current frame is speech
bool IsSpeech(Vad *vad, float *data, int num_point) {
    float energy = 0.0; 
    bool is_voice = false;
    for (int i = 0; i < num_point; i++) {
        energy += data[i] * data[i];
    }
    if (energy > vad->energy_thresh) is_voice = true;
    switch (vad->state) {
        case kSilence:
            if (is_voice) {
                vad->speech_frame_count++;
                if (vad->speech_frame_count >= vad->silence_to_speech_thresh) {
                    vad->state = kSpeech;
                    vad->silence_frame_count = 0;
                }
            } else {
                vad->speech_frame_count = 0;
            }
            break;
        case kSpeech:
            if (!is_voice) {
                vad->silence_frame_count++;
                if (vad->silence_frame_count >= vad->speech_to_sil_thresh) {
                    vad->state = kSilence;
                    vad->speech_frame_count = 0;
                }
            } else {
                vad->silence_frame_count = 0;
            }
            break;
        default:
            assert(0);
    }

    if (vad->state == kSpeech) return true;
    else return false;
}

#endif



