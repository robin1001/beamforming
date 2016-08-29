CXX = g++
CXXFLAGS = -g

BINS = wav-test fft-test tdoa-test apply-delay-and-sum vad-test

all: $(BINS)

wav-test: wav.h
fft-test: fft.h
tdoa-test: ds.h tdoa.h utils.h fft.h data.h
apply-delay-and-sum: ds.h tdoa.h utils.h fft.h wav.h
vad-test: wav.h vad.h

.PHONY: clean
clean:
	rm -f *.o $(BINS)
