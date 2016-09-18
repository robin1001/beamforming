#!/usr/bin/bash 

# Created on 2016-09-18
# Author: Zhang Binbin

./apply-delay-and-sum --tdoa-window=4000 --beam-window=4000 --margin=16 \
    4ch.wav 4ch.delay_and_sum.wav

./apply-gsc --num-k=128 --alpha=0.01 \
    4ch.wav 4ch.gsc.wav

./apply-mvdr --frame-len=0.025 --frame-shift=0.01 --fft-point=512 \
    --energy-thresh=1.5e-7 \
    --sil-to-speech-trigger=3 \
    --speech-to-sil-trigger=10 \
    4ch.wav 4ch.mvdr.wav





