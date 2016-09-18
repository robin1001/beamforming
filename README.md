# Beamforming Toolkit

Created on 2016-09-13

Here we implement three classic beamforming algorithms, eg Delay and Sum, MVDR, GSC. And
implement the classic TDOA estimate algorithms GCC-PHAT.


## Usage
apply-delay-and-sum, apply-gsc, apply-mvdr are three bins that accept one multichannel wavfile and output the corresponding single channel beamforming wav file. 

``` sh
sh run.sh
```

## File List 

tdoa.h ds.h gsc.h mvdr.h are corresponding cc implementation.
tdoa.m gsc.m mvdr.m are corresponding matlab implementation.
And if there is something wrong, it's very useful for debuging compared with matlab result.
xxx-test are some simple test program.

## Reference

Here list some refered papers.

### GCC-PHAT(general cross corelation-phase transform)
1. Microphone Array Signal Processing(chappter 9: Direction-of-Arrival and Time-Difference-of-Arrival Estimation)

### Delay and Sum
1. Microphone Array Signal Processing(chappter 3: Conventional Beamforming Techniques)
2. http://www.labbookpages.co.uk/audio/beamforming/delaySum.html

### MVDR(minimum variance distortion response)
1. Microphone Array Signal Processing(chappter 3: Conventional Beamforming Techniques)
2. Microphone Array Signal Processing(chappter 9: Direction-of-Arrival and Time-Difference-of-Arrival Estimation)
3. Performance Study of the MVDR Beamformer as a Function of the Source Incidence Angle

### GSC
1. An Algorithm For Linearly Constrained Adaptive Array Processing
2. An Alternative Approach to Linearly Constrained Adaptive Beamforming

## Problem
Here we use diy simple matrix implementation, so it is not very efficient.
And there is some numerical problems in the matrix inverse operation, 
the invertion is not as precise as matlab, and maybe fail some condition.

