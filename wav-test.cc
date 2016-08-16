/* Created on 2016-08-15
 * Author: Zhang Binbin
 */

#include "wav.h"


int main() {
    WavReader reader("2.wav");

    WavWriter writer(reader.Data(), reader.NumSample(), reader.NumChannel(),
                     reader.SampleRate(), reader.BitsPerSample());
    writer.Write("2.cpy.wav");
    return 0;
}
