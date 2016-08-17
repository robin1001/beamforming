% Copyright (c) 2004-2005, International Computer Science Institute
% and Tampere University of Technology
%
% All rights reserved.
% 
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions
% are met:
% 
% Redistributions of source code must retain the above copyright notice,
% this list of conditions and the following disclaimer.
% 
% Redistributions in binary form must reproduce the above copyright
% notice, this list of conditions and the following disclaimer in the
% documentation and/or other materials provided with the distribution.
% 
% Neither the name of ICSI and Tampere University of Technology nor
% the names of their contributors may be used to endorse or promote
% products derived from this software without specific prior written
% permission.
% 
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
% "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
% LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
% A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
% OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
% SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
% TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
% PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
% LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
% NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
% SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

function output = phat_delaysum(waves, fs, refIdx, margin)
%y = phat_delaysum(x, fs, refIdx, vMargin) returns a waveform obtained by
%delaying and summing the supplied input waves. Cross-channel time-delay 
%estimation (TDE) is performed using PHAT-weighted generalized 
%cross-correlation.
%
%Arguments:
%   x: cell array containing the input waveforms.
%   fs: sampling rate (in sps).
%   refIdx: index of the reference channel.
%   margin: search range for the maximum correlation lag (in seconds).

%This script is based on ArrayProcessor.m written by Tuomo Pirinen in spring
% 2004. It was modified by Marc Ferras in June 2005 to implement PHAT-GCC
%time-delay estimation.

%Switch waves so that the reference is always at index 1
tmp=waves{1};
waves{1}=waves{refIdx};
waves{refIdx}=tmp;

nChannels = length(waves) ; %Number of input channels
nSamples=length(waves{1});

marginSamples = round(margin*fs) ; %Margin in samples

output = zeros(nSamples,1) ; %Initialize output vector
delays=zeros(1,nChannels); %Initizalize TDE vector

%Find 2^x such that 2^x>2*nSamples
fftSize=2;
while fftSize<2*nSamples
        fftSize=fftSize*2;
end
fftSize2=floor(fftSize/2);

for(k=1:nChannels)
  %GCC-
  fft1=fft(waves{k},fftSize);
  fft2=fft(waves{1},fftSize);
  G12=fft1.*conj(fft2);
  denom=max(abs(G12),1e-6);
  G=G12./denom;
  g=real(ifft(G));
  g=fftshift(g);

  [maxVal, maxIdx] = max(g(fftSize2+1-marginSamples:fftSize2+1+marginSamples));
  delays(k) = maxIdx - marginSamples - 1;  
  
  mtxMatchSamples(k,1) = [ delays(k) + 1 ] ;
  if(mtxMatchSamples(k,1)<1)
    disp('Pre-padding')
    padLen = 1 - delays(k) ;
    waves{k} = [ zeros(padLen,1) ; waves{k} ] ;
    mtxMatchSamples(k,1) = 1 ;
    mtxMatchSamples(k,2) = mtxMatchSamples(k,2) + padLen ;
  end

  mtxMatchSamples(k,2) =  mtxMatchSamples(k,1) + nSamples - 1 ;
  if( mtxMatchSamples(k,2) > nSamples )
    disp('Post-padding') ;
    padLen = mtxMatchSamples(k,2) - nSamples + 1; 
    waves{k} = [ waves{k} ; zeros(padLen,1)  ] ;    
  end
  
  alignedWave = waves{k}(mtxMatchSamples(k,1):mtxMatchSamples(k,2));
  output = output + alignedWave ;
end

delays
output = 0.8*output./max(abs(output)) ;

return
