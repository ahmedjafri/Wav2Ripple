//
//  BPMAnalyzer.h
//  Ripple
//
//  Created by Ahmed Jafri on 1/20/15.
//  Copyright (c) 2015 Ripple. All rights reserved.
//

#ifndef __Ripple__BPMAnalyzer__
#define __Ripple__BPMAnalyzer__

#include <stdio.h>
#include <vector>

struct BPMApproximation
{
    float BPM;
    float percentageCertainty;
};

class BPMAnalyzer
{
public:
    BPMAnalyzer(const std::vector<float> energyLevels, int WindowSizeOfPCM);
    std::vector<BPMApproximation> getBPMApproximations();
    
private:
    float getBPMFromNumberOfTimesAppearingInFrequencyPercentages(int numberOfOccurencesInBuffer);
    float calculateApproximateBPMForClump(const std::vector<int> clump, const std::vector<float> frequencyPercentages);

    int analyzerWindowSizeOfPCM;
    std::vector<float> getFrequencyPercentages();
    std::vector<float> energyLevels;
};

#endif /* defined(__Ripple__BPMAnalyzer__) */
