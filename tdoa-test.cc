/* Created on 2016-08-16
 * Author: Zhang Binbin
 */

#include <stdio.h>
#include <string.h>

#include "tdoa.h"
#include "ds.h"
#include "data.h"


static void DelaySequence(float *in_data, int num, int delay, float *out_data) {
    for (int i = 0; i < num; i++) {
        if (i - delay >= 0 && i - delay < num) {
            out_data[i] = in_data[i - delay];
        }
        else {
            out_data[i] = 0;
        }
    }
}

#define NUM_CHANNEL 3

int main() {
    // When test this, first modify beamforming.h drop the hamming window
    float *delay_data1 = (float *)calloc(sizeof(float), NUM_DATA);
    float *delay_data2 = (float *)calloc(sizeof(float), NUM_DATA);
    DelaySequence(g_data, NUM_DATA, 10, delay_data1);
    DelaySequence(g_data, NUM_DATA, -5, delay_data2);
    
    float *all_data = (float *)calloc(sizeof(float), NUM_DATA * NUM_CHANNEL);
    memcpy(all_data, g_data, sizeof(float) * NUM_DATA);
    memcpy(all_data + NUM_DATA, delay_data1, sizeof(float) * NUM_DATA);
    memcpy(all_data + 2 * NUM_DATA, delay_data2, sizeof(float) * NUM_DATA);

    int tdoa[NUM_CHANNEL] = {0};
    GccPhatTdoa(all_data, NUM_CHANNEL, NUM_DATA, 0, NUM_DATA / 2, tdoa);

    for (int i = 0; i < NUM_CHANNEL; i++) {
        printf("%d\n", tdoa[i]);
    }

    float *out = (float *)calloc(sizeof(float), NUM_DATA);
    DelayAndSum(all_data, NUM_CHANNEL, NUM_DATA, tdoa, out);
    
    free(delay_data1);
    free(delay_data2);
    free(all_data);
    free(out);
    printf("Done\n");
    return 0;
}



