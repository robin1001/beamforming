/* Created on 2016-08-29
 * Author: Zhang Binbin
 * About: MVDR in cc
 */

#ifndef MVDR_H_
#define MVDR_H_

#include <vector>

#include "fft.h"
#include "utils.h"
#include "matrix.h"

// MVDR (minimum variance distortionless response filter), matlab code 
//
// function [w,variance]=mvdr_coeff(R,s)
// % Returns the coefficients for the MVDR (minimum variance distortionless 
// % response filter) for the signature vector s and correlation matrix R
//
// Rinv=inv(R);
// w=(Rinv*s)/(s'*Rinv*s);
// end


// Reference:
// 1. Microphone Array Signal Processing(chappter 3: Conventional Beamforming Techniques)
// 2. Microphone Array Signal Processing(chappter 9: Direction-of-Arrival and Time-Difference-of-Arrival Estimation)
// 3. Performance Study of the MVDR Beamformer as a Function of the Source Incidence Angle

class Mvdr {
public:
    // @params fft_point: do fft in n point(must be 2^N) 
    Mvdr(int fft_point, int num_channel): fft_point_(fft_point), 
           num_channel_(num_channel), frame_count_(0) {
        num_valid_point_ = fft_point_ / 2 + 1;
        sum_mat_.resize(num_valid_point_);         
        square_sum_mat_.resize(num_valid_point_);
        for (int i = 0; i < num_valid_point_; i++) {
            sum_mat_[i] = new ComplexMatrix(num_channel_, num_channel_);
            square_sum_mat_[i] = new ComplexMatrix(num_channel_, num_channel_);
        }
    }

    ~Mvdr() {
        for (int i = 0; i < num_valid_point_; i++) {
            delete sum_mat_[i];
            delete square_sum_mat_[i];
        }
    }
    
    // @params is_speech: 0 represent none speech otherwise speech
    void DoBeamformimg(const float *data, int num_sample, int is_speech,
        int tdoa, float *out) {
        assert(num_sample <= fft_point_);
        float *win_data = (float *)calloc(sizeof(float), fft_point_ * num_channel_); 
        // 1. copy and apply window
        for (int i = 0; i < num_channel_; i++) {
            memcpy(win_data + i * fft_point_, data + i * num_sample, 
                   sizeof(float) * num_sample);
            Hamming(win_data, num_sample);
        }

        // 2. do fft
        float *fft_real = (float *)calloc(sizeof(float), 
                                          fft_point_ * num_channel_);
        float *fft_img = (float *)calloc(sizeof(float), 
                                          fft_point_ * num_channel_);
        for (int i = 0; i < num_channel_; i++) {
            memcpy(fft_real + i * fft_point_, win_data + i * fft_point_, 
                    sizeof(float) * fft_point_);
            fft(fft_real + i * fft_point_, fft_img + i * fft_point_, 
                fft_point_);  
        }

        // update corrvar matrix(first several frames always be regard as none speech)
        if (!is_speech || frame_count_ <= 10) {

        }
        // calc s acorrding tdoa 
         
        // calc covariance matrix, and inverse
        
        // calc w
        
        // sum

        free(win_data);
    }

private:
    int fft_point_;        
    int num_channel_;
    int frame_count_;
    int num_valid_point_;
    std::vector<ComplexMatrix *> sum_mat_;
    std::vector<ComplexMatrix *> square_sum_mat_;
};

#endif
