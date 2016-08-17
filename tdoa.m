% Created on 2016-08-15
% Author: Zhang Binbin
% About: for comparision with the c code

% load gong;
% refsig = y;
% delay1 = 5;
% delay2 = 25;
% sig1 = delayseq(refsig,delay1);
% sig2 = delayseq(refsig,delay2);
% tau_est = gccphat([sig1,sig2],refsig)

pcm = audioread('2.wav');
pcm_4000 = pcm(1:4000, :) * 2^15;
window = hamming(4000);
ch1 = pcm_4000(:, 1) .* window;
ch2 = pcm_4000(:, 2) .* window;

refsig = [ ch1; zeros(96, 1) ];
sig = [ ch2; zeros(96, 1) ];

% gcc phat
fft0 = fft(sig);
fft1 = fft(refsig);
num = fft0 .* conj(fft1);
den = abs(num);
rev = ifft(num ./ den);
[maxi, max_id] = max(fftshift(rev));
half = length(refsig) / 2;
delay = max_id - half - 1


