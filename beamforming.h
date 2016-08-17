/* Created on 2016-08-15
 * Author: Zhang Binbin
 * About: beamform in c stype
 */

#ifndef BEAMFORMING_H_
#define BEAMFORMING_H_

#include <math.h>
#include <string.h>
#include <assert.h>

#include "fft.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795                                              
#endif                                                                                        

#ifndef M_2PI
#define M_2PI 6.283185307179586476925286766559005                                           
#endif  

static int nearest_power_of_two(int n) {
    return (int)pow(2, ceil(log(n) / log(2))); 
}

// Calc reference channel
void calc_reference_channel();

// Calc tdoa(time delay of arrival
// using GCC-PHAT(Gerneral Cross Correlation - Phase Transform)
// @params data : in format channel0, channel1
// @params ref : reference_channel
// @params margin: margin [-tao, tao]
void calc_tdoa(const float *data, int num_channel, int num_sample, 
               int ref, int margin, int *tdoa) {
    assert(data != NULL);
    assert(ref >= 0 && ref < num_channel);
    // constrait the number data points to 2^n
    int num_points = nearest_power_of_two(num_sample);
    int half = num_points / 2;
    float *win_data = (float *)calloc(sizeof(float), num_points * num_channel);
    float *hamming_window = (float *)calloc(sizeof(float), num_sample);
    // calc hamming window
    for (int i = 0; i < num_sample; i++) {
        hamming_window[i] = 0.54 - 0.46 * cos(M_2PI * i / (num_sample -1));
    }
    // copy data and apply window
    for (int i = 0; i < num_channel; i++) {
        for (int j = 0; j < num_sample; j++) {
            //win_data[i * num_points + j] = data[i * num_sample + j];
            win_data[i * num_points + j] = data[i * num_sample + j] * 
                                           hamming_window[j];
        }
    }

    float *fft_real = (float *)calloc(sizeof(float), num_points * num_channel);
    float *fft_img = (float *)calloc(sizeof(float), num_points * num_channel);
    // do fft
    for (int i = 0; i < num_channel; i++) {
        memcpy(fft_real + i * num_points, win_data + i * num_points, 
                sizeof(float) * num_points);
        fft(fft_real + i * num_points, fft_img + i * num_points, num_points);  
    }

    float *corr_real = (float *)calloc(sizeof(float), num_points * num_channel);
    float *corr_img = (float *)calloc(sizeof(float), num_points * num_channel);
    // do gcc-phat
    for (int i = 0; i < num_channel; i++) {
        if (i != ref) {
            // * do fft cross correlation, fft(i) fft(ref)*
            // (a + bj) (c + dj)* = (a + bj) (c - dj) = (ac + bd) + (bc - ad)j
            for (int j = 0; j < num_points; j++) {
                int m = ref * num_points + j, n = i * num_points + j;
                corr_real[n] = fft_real[n] * fft_real[m] + fft_img[n] * fft_img[m];
                corr_img[n] = fft_img[n] * fft_real[m] - fft_real[n] * fft_img[m];
                float length = sqrt(corr_real[n] * corr_real[n] + 
                                    corr_img[n] * corr_img[n]);
                corr_real[n] /= length;
                corr_img[n] /= length;
            }
            // * do inserse fft
            fft(corr_real + i * num_points, corr_img + i * num_points, -num_points);
            // * rearange idft index(fftshift), make num_points / 2 as the center
            for (int j = 0; j < half; j++) {
                float t = corr_real[i * num_points + j];
                corr_real[i * num_points + j] = corr_real[i * num_points + j + half];
                corr_real[i * num_points + j + half] = t;
            }
            // * select max
            int max_j = half - margin;
            assert(max_j >= 0);
            float max = corr_real[i * num_points + max_j]; 
            for (int j = half - margin; j < half + margin; j++) {
                if (corr_real[i * num_points + j] > max) {
                    max = corr_real[i * num_points + j];
                    max_j = j;
                }
            }
            tdoa[i] = max_j - half;
        }
        else {
            tdoa[i] = 0;
        }
    }

    // free all
    free(win_data);
    free(hamming_window);
    free(fft_real);
    free(fft_img);
    free(corr_real);
    free(corr_img);
}

// Delay & Sum
// @params data : in format channel0, channel1
void delay_and_sum(const float *data, int num_channel, int num_sample,
                   int *tdoa, float *out) {
    for (int i = 0; i < num_sample; i++) {
        int count = 0;
        float sum = 0.0;
        for (int j = 0; j < num_channel; j++) {
            if (i + tdoa[j] >= 0 && i + tdoa[j] < num_sample) {
                sum += data[j * num_sample + i + tdoa[j]];
                count++;
            }
        }
        assert(count > 0);
        out[i] = sum / count;
    }
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

void mvdr(const float *data, int num_channel, int num_sample,
          int *tdoa, float *out) {

}


#endif
