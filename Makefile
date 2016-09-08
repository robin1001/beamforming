CXX = g++
CXXFLAGS = -g

BINS = wav-test fft-test tdoa-test  vad-test matrix-test \
       apply-delay-and-sum apply-mvdr apply-gsc

all: $(BINS)

wav-test: wav.h
fft-test: fft.h
tdoa-test: ds.h tdoa.h utils.h fft.h data.h
vad-test: wav.h vad.h
matrix-test: matrix.h
apply-delay-and-sum: ds.h tdoa.h utils.h fft.h wav.h
apply-mvdr: mvdr.h matrix.h tdoa.h utils.h fft.h wav.h
apply-gsc: gsc.h matrix.h utils.h wav.h

.PHONY: clean
clean:
	rm -f *.o $(BINS)
