/* Created on 2016-08-15
 * Author: Zhang Binbin
 */

#include "wav.h"


int main(int argc, char *argv[]) {

    const char *usage = "Test wav reader and writer\n"
                        "Usage: wav-test wav_in_file wav_output_file\n";
    if (argc != 3) {
        printf(usage);
        exit(-1);
    }

    WavReader reader(argv[1]);

    WavWriter writer(reader.Data(), reader.NumSample(), reader.NumChannel(),
                     reader.SampleRate(), reader.BitsPerSample());
    writer.Write(argv[2]);
    return 0;
}
