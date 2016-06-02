//
//  WavReader.hpp
//  RippleEngine
//
//  Created by Ahmed Jafri on 6/1/16.
//  Copyright © 2016 Ripple. All rights reserved.
//

#ifndef WavReader_h
#define WavReader_h

#include <stdio.h>
#include "wave.h"
#include "RippleReader.h"

class WavReader : public RippleReader {
public:
    //returns a float array of the song from a start index and length
    std::vector<float> read(int start, int length);
    int numberOfSamples();
    WavReader(const char * pathToWavFile);
    WavReader();
    long sampleRate();
private:
    const char * file;
    WAVE_HEADER header;
    int headerOffset;
    long num_samples;
    long size_of_each_sample;
    float duration_in_seconds;
};

#endif /* WavReader_h */
