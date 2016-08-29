/* Created on 2016-08-15
 * Author: Zhang Binbin
 * About: utils tool for beamforming, eg macro, window etc
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif                                                                                        
#ifndef M_2PI
#define M_2PI 6.283185307179586476925286766559005
#endif

int UpperPowerOfTwo(int n) {
    return (int)pow(2, ceil(log(n) / log(2))); 
}

// Apply hamming window on data in place
void Hamming(float *data, int num_point) {
    for (int i = 0; i < num_point; i++) {
        data[i] = data[i] * (0.54 - 0.46 * cos(M_2PI * i / (num_point-1)));  
    }
}

#endif
