/* Created on 2016-08-29
 * Author: Zhang Binbin
 * About: MVDR in c
 */

#ifndef MVDR_H_
#define MVDR_H_


// TODO MVDR beamforming
// MVDR (minimum variance distortionless response filter), matlab code 
//
// function [w,variance]=mvdr_coeff(R,s)
// % Returns the coefficients for the MVDR (minimum variance distortionless 
// % response filter) for the signature vector s and correlation matrix R
//
// Rinv=inv(R);
// w=(Rinv*s)/(s'*Rinv*s);

void Mvdr(const float *data, int num_channel, int num_sample,
          int *tdoa, float *out) {
    // calc s acorrding tdoa 
    
    // calc covariance matrix, and inverse
    
    // calc w
    
    // sum
}




