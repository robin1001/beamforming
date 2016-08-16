CXX = g++
CXXFLAGS = -g

TEST_BINS = wav-test fft-test

all: $(TEST_BINS)

wav-test: wav.h
fft-test: fft.h
