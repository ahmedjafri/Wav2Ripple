//
//  BPMAnalyzer.cpp
//  Ripple
//
//  Created by Ahmed Jafri on 1/20/15.
//  Copyright (c) 2015 Ripple. All rights reserved.
//

#include "BPMAnalyzer.h"
#include "TimeToFrequencyTransformer.h"
#include "RPCommonDSPFunctions.h"
#include "Globals.h"

#include <algorithm>

#define SECONDS_IN_A_MINUTE 60


BPMAnalyzer::BPMAnalyzer(const std::vector<float> energyLevels, int windowSizeOfPCM):
energyLevels(energyLevels),
analyzerWindowSizeOfPCM(windowSizeOfPCM)
{

}

float BPMAnalyzer::calculateApproximateBPMForClump(const std::vector<int> clump, const std::vector<float> frequencyPercentages)
{
    float frequencyTotal = 0.0f;
    float percentageTotal = 0.0f;
    
    for(int i=0; i < clump.size(); i++)
    {
        frequencyTotal += frequencyPercentages[clump[i]]*getBPMFromNumberOfTimesAppearingInFrequencyPercentages(clump[i]);
        percentageTotal += frequencyPercentages[clump[i]];
    }
    
    return frequencyTotal / percentageTotal;
}

std::vector<BPMApproximation> BPMAnalyzer::getBPMApproximations()
{
    std::vector<float> frequencyPercentages = getFrequencyPercentages();

    std::vector<BPMApproximation> returnVector = std::vector<BPMApproximation>();
    
    
    int lowestNumberOfOccurences = 10; // around 50 BPM
    int highestNumberOfOccurences = 60; // around 300 BPM
    
    std::vector<float>frequencyPercentagesSubVector(frequencyPercentages.begin()+lowestNumberOfOccurences, frequencyPercentages.begin()+highestNumberOfOccurences);
    std::vector<std::vector<int>> clumps = getClumpIndexes(frequencyPercentagesSubVector, 2, 0.1);
    
    
    for(int i = 0; i < clumps.size(); i++)
    {
        std::vector<int> clump = clumps[i];

        transform(clump.begin(), clump.end(), clump.begin(),
                  bind2nd(std::plus<float>(), lowestNumberOfOccurences)); // offset indexes due to using a sub vector
        
        BPMApproximation approximation;
        approximation.BPM = calculateApproximateBPMForClump(clump, frequencyPercentages);
        approximation.percentageCertainty = getArrayMean(&frequencyPercentages[clump[0]], clump.size()) * 100.0f;
        
        returnVector.push_back(approximation);
    }
    
    return returnVector;
}

float BPMAnalyzer::getBPMFromNumberOfTimesAppearingInFrequencyPercentages(int numberOfOccurencesInBuffer)
{
#if __APPLE__
#warning this is a magic number derived from the number found in the RPAnalyzer in the getFrequencyPercentages function
#endif
	int bufferSize = 512;
    
    return (float)SECONDS_IN_A_MINUTE/(((float)analyzerWindowSizeOfPCM/(float)SAMPLE_RATE) * ((float)bufferSize/(float)numberOfOccurencesInBuffer));
}

std::vector<float> BPMAnalyzer::getFrequencyPercentages()
{
    int lookAheadWindows = 512; // size of the FFT window to be returned
    
    std::vector<float> ShiftingWindow = std::vector<float>(lookAheadWindows,0.0f);
    
    for (int i = 0; i < energyLevels.size() - lookAheadWindows; i+= lookAheadWindows)
    {
        lookAheadWindows = energyLevels.size() - i >= 512 ? 512 : energyLevels.size();
        float* energyBufferPart = &energyLevels[i];
        
        TimeToFrequencyTransformer transformer = TimeToFrequencyTransformer(energyBufferPart, lookAheadWindows);
        
        transformer.windowed = false;
        std::vector<float> BPMGraphValues = transformer.getFrequencyBand();
        BPMGraphValues[0] = 0;
        BPMGraphValues[1] = 0;
        BPMGraphValues[2] = 0;
        BPMGraphValues[3] = 0;
        BPMGraphValues[4] = 0;
        BPMGraphValues[5] = 0;
        BPMGraphValues[6] = 0;
        
        int numberOfMaxValues = 6;
        std::vector<int> maxIndexes = findMaxIndexes(BPMGraphValues.data(), transformer.frequencyBandLength*3/4, numberOfMaxValues);
        
        for (int j = 0; j < numberOfMaxValues; j++)
        {
            ShiftingWindow[maxIndexes[j]]++;
        }
    }
    
    normalizeArray(ShiftingWindow.data(), lookAheadWindows, 1);
    
    return ShiftingWindow;
}
