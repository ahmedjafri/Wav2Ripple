//
//  TimeToFrequencyTransformer.h
//  Ripple
//
//  Created by Ahmed Jafri on 11/25/14.
//  Copyright (c) 2014 Ripple. All rights reserved.
//

#ifndef __Ripple__TimeToFrequencyTransformer__
#define __Ripple__TimeToFrequencyTransformer__

#include <stdio.h>
#include <vector>

#if __APPLE__
#include <Accelerate/Accelerate.h>
#else
#include "fft/tools/kiss_fftr.h"
#endif

#include <mutex>

typedef struct
{
    float* buffer;
    int number_of_bins;
} FrequencyBandBuffer;

//-----------------------------------------------
// Finds the next power of 2
//
static inline int nextp2(int x)
{
    int val = 1;
    while (val < x)
        val <<= 1;
    return val;
}

class TimeToFrequencyTransformer
{
public:
    TimeToFrequencyTransformer(const float* timeDomainSignal, const size_t signalLength);
    ~TimeToFrequencyTransformer();
    
    void assignNewTimeSignal(const float* timeDomainSignal);
    bool windowed;
    int frequencyBandLength;
    std::vector<float> getFrequencyBand();
    std::vector<float> convertBackToTime(std::vector<int> includedBins);
    float* timeBuffer;
    float* frequencyBuffer;
private:
    int fftSize;
    std::vector<float> window;
    std::vector<float> fftBuffer;
    size_t timeSignalLength;
    
    
#if __APPLE__
    FFTSetup fft_weights;
    vDSP_Length log2n;
    DSPSplitComplex inputData;
#else
	kiss_fft_cpx* out;
#endif
};

#endif /* defined(__Ripple__TimeToFrequencyTransformer__) */
