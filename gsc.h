/* Created on 2016-09-07
 * Author: Zhang Binbin
 * About: GSC(generalized sidelobe canceling) implemention
 */


#ifndef GSC_H_
#define GSC_H_

#include <assert.h>

#include "matrix.h"

// Reference:
// 1. An Algorithm For Linearly Constrained Adaptive Array Processing
// 2. An Alternative Approach to Linearly Constrained Adaptive Beamforming

class Gsc {
public:
    Gsc(int num_channel, int num_k, float alpha): num_channel_(num_channel), 
            num_k_(num_k), alpha_(alpha), 
            b_(num_channel - 1, num_channel),
            w_(num_channel - 1, num_k) {
        assert(b_.NumRow() + 1 == b_.NumCol()); 
        for (int i = 0; i < b_.NumRow(); i++) {
            b_(i, i) = 1;
            b_(i, i+1) = -1;
        }
    }

    ~Gsc() {}
    
    // Here we suppose multichannel data is already aligned  
    void DoBeamformimg(const float *data, int num_sample, float *out) {
        assert(num_sample == num_k_);
        // init x
        x_.Resize(num_channel_, num_k_);
        for (int i = 0; i < num_channel_; i++) {
            for (int j = 0; j < num_k_; j++) {
                x_(i, j) = data[i * num_k_ + j];
            }
        }
        // yu, upper y in gsc graph(current fixed beamforming result)
        float yu = 0.0;
        for (int i = 0; i < num_channel_; i++) {
            yu += x_(i, num_k_ - 1);
        }
        yu /= num_channel_;

        //Matrix y, z, s; // scale normal factor
        z_.Mul(b_, x_); // z = b_ * x_
        y_.MulElement(w_, z_); // y_ = w_ .* z_;
        s_.MulElement(z_, z_);

        float yl = y_.Sum(); // lower y
        float norm = s_.Sum(); // norm factor
        *out = yu - yl; // gsc
        // w(t+1) = w(t) + alpha * y(t) * z(t)
        w_.Add(z_, alpha_ / norm * (*out));
    }

private:
    int num_channel_;
    int num_k_;
    float alpha_;
    Matrix b_, w_;
    Matrix x_, y_, z_, s_; // auxiliary matrix
};


#endif
