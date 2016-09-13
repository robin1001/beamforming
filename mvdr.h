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
    Mvdr(int sample_rate, int fft_point, int num_channel): 
            sample_rate_(sample_rate), 
            num_channel_(num_channel), 
            fft_point_(fft_point), 
            frame_count_(0) {
        num_valid_point_ = fft_point_ / 2 + 1;
        global_covars_.resize(num_valid_point_);         
        local_covars_.resize(num_valid_point_);
        w_.resize(num_valid_point_);
        for (int i = 0; i < num_valid_point_; i++) {
            global_covars_[i] = new ComplexMatrix(num_channel_, num_channel_);
            local_covars_[i] = new ComplexMatrix(num_channel_, num_channel_);
            w_[i] = new ComplexMatrix(num_channel_, 1);
        }
    }

    ~Mvdr() {
        for (int i = 0; i < num_valid_point_; i++) {
            delete global_covars_[i];
            delete local_covars_[i];
            delete w_[i];
        }
    }
    
    // @params is_speech: 0 represent none speech otherwise speech
    void DoBeamformimg(const float *data, int num_sample, bool is_noise,
            float *tdoa, float *out) {
        assert(num_sample <= fft_point_);
        frame_count_++;
        float *win_data = (float *)calloc(sizeof(float), fft_point_ * num_channel_); 
        // 1. copy and apply window
        for (int i = 0; i < num_channel_; i++) {
            memcpy(win_data + i * fft_point_, data + i * num_sample, 
                   sizeof(float) * num_sample);
            Hamming(win_data + i * fft_point_, num_sample);
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
        
        // 3. calc and update global noise variance(when noise or flat start)
        if (is_noise || frame_count_ < 100) {
            ComplexMatrix spectrum_t(num_channel_, 1), spectrum_c(1, num_channel_);
            for (int i = 0; i < num_valid_point_; i++) {
                for (int j = 0; j < num_channel_; j++) {
                    spectrum_t(j, 0).real = fft_real[j * fft_point_ + i];
                    spectrum_t(j, 0).img = fft_img[j * fft_point_ + i];
                    spectrum_c(0, j).real = fft_real[j * fft_point_ + i];
                    spectrum_c(0, j).img = -fft_img[j * fft_point_ + i];
                }
                local_covars_[i]->Mul(spectrum_t, spectrum_c);
                float scale = 1.0 / local_covars_[i]->Trace();
                local_covars_[i]->Scale(scale);

                // update global covar
                // 3.1 mean update
                float lr = 1.0 / frame_count_;
                global_covars_[i]->Add(*local_covars_[i], lr, 1 - lr);
                // 3.2 learn rate update
                //float lr = 0.01;
                //global_covars_[i]->Add(local_covars_[i], lr);
            }
        }

        // 4. MVDR
        ComplexMatrix alpha(num_channel_, 1), alpha_tc(1, num_channel_);
        ComplexMatrix inv(num_channel_, num_channel_);
        ComplexMatrix beta(num_channel_, 1), den(1, 1), den_inv(1, 1);
        ComplexMatrix ceil_covar(num_channel_, num_channel_);
        for (int i = 0; i < num_valid_point_; i++) {
            float f = i * sample_rate_ / fft_point_;
            // calc alpha acorrding to tdoa 
            for (int j = 0; j < num_channel_; j++) {
                alpha(j, 0).real = cos(M_2PI * f * tdoa[j]);
                alpha(j, 0).img = -sin(M_2PI * f * tdoa[j]);
                alpha_tc(0, j).real = cos(M_2PI * f * tdoa[j]);
                alpha_tc(j, 0).img = sin(M_2PI * f * tdoa[j]);
            }
            // inverse covariance matrix
            //global_covars_[i]->ApplyDiagCeil(1e-4);
            ceil_covar.Copy(*global_covars_[i]);
            ceil_covar.ApplyDiagCeil(1e-4);
            inv.Inverse(ceil_covar);
            //inv.Print();
            // calc inv * alpha
            beta.Mul(inv, alpha);
            den.Mul(alpha_tc, beta);
            den_inv.Inverse(den);
            // w=(Rinv*s)/(s'*Rinv*s);
            w_[i]->Mul(beta, den_inv); // mvdr
            //w_[i]->Print(); // mvdr
        }

        // 5. sum 
        {
            // 5.1 conj(w_) .* s
            for (int j = 0; j < num_channel_; j++) {
                for (int i = 0; i < num_valid_point_; i++) {
                    float a = fft_real[j * fft_point_ + i], 
                          b = fft_img[j * fft_point_ + i], // conj
                          c = (*w_[i])(j, 0).real,
                          d = -(*w_[i])(j, 0).img;

                    fft_real[j * fft_point_ + i] = a * c - b * d;
                    fft_img[j * fft_point_ + i] = a * d + b * c;
                }
            }
            // 5.2 add to channel 0
            for (int i = 0; i < num_valid_point_; i++) {
                for (int j = 1; j < num_channel_; j++) {
                    fft_real[0 * fft_point_ + i] += fft_real[j * fft_point_ + i];
                    fft_img[0 * fft_point_ + i] += fft_img[j * fft_point_ + i];
                }
            }
            // 5.3 symmetry
            for (int i = num_valid_point_; i < fft_point_; i++) {
                fft_real[i] = fft_real[fft_point_ - i];
                fft_img[i] = -fft_img[fft_point_ - i];
            }
        }

        // 6. ifft & rewindow & overlap-and-add 
        fft(fft_real, fft_img, -fft_point_);
        Hamming(fft_real, num_sample); // rewindow
        for (int i = 0; i < num_sample; i++) {
            out[i] += fft_real[i]; // overlap-and-add
        }
        
        free(fft_real);
        free(fft_img);
        free(win_data);
    }

private:
    int sample_rate_;
    int num_channel_;
    int fft_point_;        
    int frame_count_;
    int num_valid_point_;
    std::vector<ComplexMatrix *> global_covars_, local_covars_;
    std::vector<ComplexMatrix *> w_;
};

#endif
