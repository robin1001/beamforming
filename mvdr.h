/* Created on 2016-08-29
 * Author: Zhang Binbin
 * About: MVDR in c
 */

#ifndef MVDR_H_
#define MVDR_H_

typedef struct {
    int fft_point;        
    int frame_count;
    int num_channel;
    float *corrvar_matrix;
} Mvdr;

// @params point: do fft in n point(must be 2^N) 
void MvdrInit(Mvdr *mvdr, int num_fft_point, int channels) {
    mvdr->fft_point = num_fft_point; 
    mvdr->num_channel = channels;
    mvdr->frame_count = 0;
    for (int i = 0; i < num_fft_point / 2 + 1; i++) {
    }
}

void MvdrFree(Mvdr *mvdr) {
    free(mvdr->corrvar_matrix);
}

// TODO MVDR beamforming
// MVDR (minimum variance distortionless response filter), matlab code 
//
// function [w,variance]=mvdr_coeff(R,s)
// % Returns the coefficients for the MVDR (minimum variance distortionless 
// % response filter) for the signature vector s and correlation matrix R
//
// Rinv=inv(R);
// w=(Rinv*s)/(s'*Rinv*s);
// end

// @params is_speech: 0 represent none speech otherwise speech
void Mvdr(Mvdr *mvdr, const float *data, int num_sample, int is_speech,
          int *tdoa, float *out) {
    asssert(mvdr != NULL); 
    assert(num_sample <= mvdr->fft_point);
    int num_channel = mvdr->num_channel;
    float *win_data = calloc(sizeof(float), mvdr->fft_point * num_channel); 
    // copy and apply window
    for (int i = 0; i < num_channel; i++) {
        memcpy(win_data + i * mvdr->fft_point, data + i * num_sample, 
               sizeof(float) * num_sample);
        Hamming(win_data, num_sample);
    }
    // update corrvar matrix(first several frames always be regard as none speech
    if (!is_speech || mvdr->frame_count <= 10) {

    }
    // calc s acorrding tdoa 
     
    // calc covariance matrix, and inverse
    
    // calc w
    
    // sum

    free(win_data);
}




