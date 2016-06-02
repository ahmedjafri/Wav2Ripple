//
//  TimeToFrequencyTransformer.cpp
//  Ripple
//
//  Created by Ahmed Jafri on 11/25/14.
//  Copyright (c) 2014 Ripple. All rights reserved.
//

#include "TimeToFrequencyTransformer.h"
#include <stdlib.h>

TimeToFrequencyTransformer::TimeToFrequencyTransformer(const float* timeDomainSignal, const size_t signalLength):
timeSignalLength(signalLength),
windowed(false),
frequencyBandLength(nextp2((int)signalLength)/2 + 1)
{
    timeBuffer = (float*)malloc(signalLength * sizeof(float));
    memcpy(timeBuffer, timeDomainSignal, signalLength * sizeof(float));
    
    frequencyBuffer = (float*)calloc(frequencyBandLength, sizeof(float));
    
    fftSize = frequencyBandLength * 2.0f;

#if __APPLE__
	log2n = log2f(fftSize);
    fft_weights = vDSP_create_fftsetup(log2n, FFT_RADIX2);
#endif

    //Hamming window parameters
    float a = 25.0f/46.0f;
    float b = 21.0f/46.0f;
    
    window.reserve(timeSignalLength);
    
    for(int i = 0; i < timeSignalLength; i++)
    {
        int bin = i + (fftSize - (int)timeSignalLength)/2;

        window[i] = a - b*cosf( ( 2.0f*M_PI*bin ) / ( fftSize - 1 ) );
    }
    
#if __APPLE__
    inputData.realp = (float *)malloc(timeSignalLength * sizeof(float));
    inputData.imagp = (float *)malloc(timeSignalLength * sizeof(float));
#else
    kiss_fft_cpx* out = (kiss_fft_cpx*)std::malloc((fftSize / 2 + 1) * sizeof(kiss_fft_cpx));
#endif
    
    fftBuffer = std::vector<float>(fftSize*2, 0.0f);
}

void TimeToFrequencyTransformer::assignNewTimeSignal(const float *timeDomainSignal)
{
    memcpy(timeBuffer, timeDomainSignal, timeSignalLength * sizeof(float));
}

TimeToFrequencyTransformer::~TimeToFrequencyTransformer()
{
    free(timeBuffer);
    free(frequencyBuffer);
#if __APPLE__
    free(inputData.realp);
    free(inputData.imagp);
#else
    free(out);
#endif
}

std::vector<float> TimeToFrequencyTransformer::getFrequencyBand()
{
    
    for(int i = 0; i < timeSignalLength; i++)
    {
        int bin = i + (fftSize - (int)timeSignalLength)/2;
        fftBuffer[bin] = timeBuffer[i] * window[i];
    }
    
    std::vector<float> returnVector(frequencyBandLength + 1,0.0f);

#if __APPLE__
    vDSP_ctoz((COMPLEX*)fftBuffer.data(), 2, &inputData, 1, timeSignalLength);
    
    vDSP_fft_zrip(fft_weights, &inputData, 1, log2n, FFT_FORWARD);

    for(int i = 0; i < frequencyBandLength; i++){
        returnVector[i] = sqrt(inputData.realp[i]*inputData.realp[i]+inputData.imagp[i]*inputData.imagp[i]);
    }
#else

    kiss_fftr_cfg cfg;
    kiss_fft_scalar *in = fftBuffer.data();
    
    if ((cfg = kiss_fftr_alloc(fftSize, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
    {
        size_t i;
        
        kiss_fftr(cfg, in, out);
        free(cfg);
        
        for (i = 0; i < fftSize; i++)
        {
            if (i < fftSize / 2 + 1)
            {
                float energy = sqrtf(out[i].r*out[i].r + out[i].i*out[i].i);
                frequencyBuffer[i] += energy;
                returnVector[i] = energy;
            }
        }
    }
    else
    {
        printf("not enough memory?\n");
        exit(-1);
    }
#endif
    
    return returnVector;
}

std::vector<float> TimeToFrequencyTransformer::convertBackToTime(std::vector<int> includedBins)
{
    //return timeBuffer;
    /*DSPSplitComplex fftInputTemp;

    fftInputTemp.realp = (float *)calloc(fftSize/2, sizeof(float));
    fftInputTemp.imagp = (float *)calloc(fftSize/2, sizeof(float));
    
    if(includedBins.size() > 0)
    {
        for (std::vector<int>::iterator iter = includedBins.begin(); iter != includedBins.end(); iter++)
        {
            int bin = *iter;
            fftInputTemp.realp[bin] = fftInput.realp[bin];
            fftInputTemp.imagp[bin] = fftInput.imagp[bin];
        }
    }
    
    vDSP_fft_zrip(weights, &fftInputTemp, 1, log2f(fftSize), FFT_INVERSE);
    vDSP_ztoc(&fftInputTemp, 1, (COMPLEX*)timeBuffer, 2, fftSize/2);
    
    for (int i = 0; i < fftSize; i++) {
        timeBuffer[i] = timeBuffer[i] * 0.5/fftSize;
    }
    
    free(fftInputTemp.realp);
    free(fftInputTemp.imagp);*/

    //std::vector<float> returnVector(timeBuffer, timeBuffer+fftSize);
    
    std::vector<float> returnVector(fftSize);
    return returnVector;
}
