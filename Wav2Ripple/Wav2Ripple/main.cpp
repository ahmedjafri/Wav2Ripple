//
//  main.cpp
//  Wav2Ripple
//
//  Created by Ahmed Jafri on 5/27/16.
//  Copyright © 2016 Ripple. All rights reserved.
//

#include <iostream>
#include "../../RippleEngine/RippleEngine/dsp/RPAnalyzer.h"
#include "../../RippleEngine/RippleEngine/WavReader.h"
#include <vector>
#include <cstdio>

int main(int argc, const char * argv[]) {
    if (argc <= 1)
    {
        printf("Pass in the path to the wav file.\n");
        exit(1);
    }

    const char * filePath = argv[1];

    RippleReader* reader = new WavReader(filePath);
    
    std::vector<float> samples = reader->read(100000, 1000);
    
    for(int i = 0; i < samples.size(); i++) {
        printf("Sample %i: %f \n", i, samples[i]);
    }
    
}
