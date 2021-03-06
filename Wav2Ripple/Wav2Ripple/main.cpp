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
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, const char * argv[]) {
    if (argc <= 1)
    {
        printf("Pass in the path to the wav file.\n");
        exit(1);
    }

    const char * filePath = argv[1];

    RippleReader* reader = new WavReader(filePath);
    
    std::vector<float> samples = reader->read(100000, 1000);
    
    std::function<void (const float)> trackProgress = [](const float percentage){
        int p = 100.0 * percentage;
        if(p % 5 == 10) {
            printf("Analyzing: %d\n", p);
        }
    };
    
    RPAnalyzer analyzer = RPAnalyzer(*reader, 256, 1024, trackProgress);
    
    std::vector<float> intensities = analyzer.convertToIntensity();
    
    std::ostringstream buff;
    
    buff << "[";
    
    for(int i = 0; i < intensities.size(); i++) {
        buff << "\"" << intensities[i] << "\"";
        if(i != (intensities.size() - 1))
            buff << ",";
    }
    
    buff << "]";
    
    ofstream newFile("intensities.json");
    
    if(newFile.is_open())
    {
        newFile << buff.str();
    }
    else
    {
        //You're in trouble now Mr!
    }
    
    
    newFile.close();
}
