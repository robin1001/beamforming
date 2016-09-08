/* Created on 2016-08-15
 * Author: Zhang Binbin
 * About: gcc-phat tdoa implemention in c style
 */

#ifndef TDOA_H_
#define TDOA_H_

#include <string.h>
#include <assert.h>

#include "utils.h"
#include "fft.h"

// Reference:
// 1. Microphone Array Signal Processing(chappter 9: Direction-of-Arrival and Time-Difference-of-Arrival Estimation)

// Calc tdoa(time delay of arrival
// using GCC-PHAT(Gerneral Cross Correlation - Phase Transform)
// @params data : in format channel0, channel1
// @params ref : reference_channel
// @params margin: margin [-tao, tao]
void GccPhatTdoa(const float *data, int num_channel, int num_sample, 
               int ref, int margin, int *tdoa) {
    assert(data != NULL);
    assert(ref >= 0 && ref < num_channel);
    assert(margin <= num_sample / 2);
    // constrait the number data points to 2^n
    int num_points = UpperPowerOfTwo(num_sample);
    int half = num_points / 2;
    float *win_data = (float *)calloc(sizeof(float), num_points * num_channel);

    // copy data and apply window
    for (int i = 0; i < num_channel; i++) {
        memcpy(win_data + i * num_points, data + i * num_sample, 
               sizeof(float) * num_sample);
        Hamming(win_data, num_sample);
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
    free(fft_real);
    free(fft_img);
    free(corr_real);
    free(corr_img);
}

#endif
