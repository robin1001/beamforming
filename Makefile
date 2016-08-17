CXX = g++
CXXFLAGS = -g

BINS = wav-test fft-test beamforming-test delay-and-sum

all: $(BINS)

wav-test: wav.h
fft-test: fft.h
beamforming-test: beamforming.h data.h
delay-and-sum: beamforming.h fft.h wav.h

.PHONY: clean
clean:
	rm -f *.o $(BINS)
