% Created on 2016-09-07
% Author: Zhang Binbin
% About: GSC matlab code

[pcm, fs] = audioread('4ch.wav');
[num_point, num_channel] = size(pcm);
pcm = pcm .* 2^15;
K = 256;
alpha = 0.1;

B = zeros(num_channel-1, num_channel);
for i = 1 : num_channel - 1
    B(i, i) = 1;
    B(i, i+1) = -1;
end

bf = sum(pcm, 2) / num_channel;
out = zeros(num_point, 1);
w = zeros(num_channel - 1, K);

% init first K point
out(1:K) = bf(1:K);

for i = K : num_point
    x = pcm(i - K + 1: i, :);
    z = B * x';
    y = w .* z;
    yl = sum(sum(y)); % yl lower y
    yn = bf(i) - yl;
    u = alpha / sum(sum(z .* z));
    w = w + u * yn * z;
    out(i) = yn;
end

audiowrite('4ch.gsc256.wav', out ./ 2^15, fs);

