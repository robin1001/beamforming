% Created on 2016-08-31
% Author: Zhang Binbin
% About: MVDR matlab code
% Modified by Sining Sun 
% clear all;

%[pcm, fs] = audioread('2.wav');
[pcm, fs] = audioread('4ch.wav');
[num_point, num_channel] = size(pcm);
pcm = pcm .* 2^15;
frame_len = 400; % 
stft_len = 512;
frame_shift = 160;
learn_rate = 0.1;
frame_num = floor((num_point - frame_len) / frame_shift + 1);
output = zeros(num_point, 1);
frame_count = 1;
use_flat_start = 0;  % if use first serveral frames for global noise covariance matrix estimate
num_stat = 129; % number of frames for global noise covariance matrix estimate in use_flat_start

global_covar = zeros(num_channel, num_channel, stft_len /2 + 1);

if use_flat_start == 1
    corrvar = zeros(num_channel, num_channel, stft_len / 2 + 1, num_stat);
    for j = 1:num_stat
        sub_data = pcm((j-1)*frame_shift+1: (j-1)*frame_shift+frame_len, :).* repmat(hamming(frame_len), 1, num_channel);
        spectrum = fft(sub_data, stft_len);
        for k = 1 : stft_len / 2 + 1
            corrvar(:,:,k, j) = spectrum(k, :).' * conj(spectrum(k,: ));
            corrvar(:, :, k, j) = corrvar(:, :, k, j) / trace(corrvar(:, :, k, j));
        end
    end
    global_covar = mean(corrvar, 4);
end


 
for j = 1:frame_shift:num_point
    if j + frame_len > num_point 
         break; 
    end
    
    % vad
    is_noise = 0;
    data = pcm(j : j + frame_len -1 , :);
    energy = sum(data(:, 1).^2);
    if energy < 5e7
        is_noise = 1;
    end
    vad_res(frame_count) = is_noise;
    
    % fft 
    win_data = data .* repmat(hamming(frame_len), 1, num_channel);
    spectrum = fft(win_data, stft_len);
    
    % update covar
    if (is_noise || frame_count < num_stat) && use_flat_start == 0
        % calc covar
        covar = zeros(num_channel, num_channel, stft_len /2 + 1);
        for k = 1 : stft_len / 2 + 1
            covar(:, :, k) = spectrum(k, :).' * conj(spectrum(k, :));
            covar(:, :, k) = covar(:, :, k) / trace(covar(:, :, k));
            global_covar(:, :, k) = (frame_count - 1) / frame_count * global_covar(:, :, k) + covar(:, :, k) / frame_count;
        end
        
        % update rule 1) mean when frame_count < num_stat and learn
        % otherwise
%         if frame_count < num_stat % update sequential mean
%             global_covar = (frame_count - 1) / frame_count * global_covar + covar / frame_count;
%         else
%             global_covar = (1 - learn_rate) * global_covar + learn_rate * covar;
%         end
        % update rule 2) always use mean
        %global_covar = (frame_count - 1) / frame_count * global_covar + covar / frame_count;
    end   

    % calc w from MVDR
%     tdoa = gccphat(win_data, win_data(:,1));
%     time = tdoa / fs; 
    time = zeros(1, num_channel);
    w = zeros(num_channel, stft_len / 2 + 1);
    for k = 0 : stft_len / 2
        f = k * fs / stft_len;
        alpha = exp(-i * 2 * pi * f * time).';
        % 1) scale
        r_inv = inv(global_covar(:, :, k+1) + (1e-4) * diag(ones(num_channel, 1)));
        %r_inv = inv(global_covar(:, :, k+1) + 1e-4);
        % 2) do svd decompose
%         [u,s,v] = svd(global_covar(:, :, k));
%         for n = 1 : length(s)
%             if abs(s(n)) < abs((s(1, 1) * 1e-4)) 
%                 s(n, n) = s(1, 1) * 1e-4;
%             end
%         end
%         r_inv = v * inv(s) * conj(u');
        
        w(:, k+1) = r_inv * alpha / (conj(alpha.') * r_inv * alpha); % MVDR
    end
    
    % 3. sum signal
    rec_signal = conj(w.') .* spectrum(1:stft_len / 2 + 1, :);
    rec_signal = [rec_signal; conj(flipud(rec_signal(2: end - 1, :)))];
    res = real(ifft(sum(rec_signal, 2)));
    res = res(1:frame_len);
    % output(j:j + frame_len - 1, :) = output(j : j + frame_len -1, :) + res;
    output(j:j + frame_len - 1, :) = output(j : j + frame_len -1, :) + res .* hamming(frame_len);
    
    frame_count = frame_count + 1;
end

%audiowrite('2.mvdr_vad.global.wav', output ./ 2^15, fs);
audiowrite('4ch.adapt.mvdr.wav', output ./ 2^15, fs);
