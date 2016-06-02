//
//  RPCommonDSPFunctions.cpp
//  Ripple
//
//  Created by Ahmed Jafri on 1/20/15.
//  Copyright (c) 2015 Ripple. All rights reserved.
//

#include "RPCommonDSPFunctions.h"
#include <deque>
#include <vector>

#include <math.h>
#include <algorithm>
#include <numeric>
#include <assert.h>

void normalizeArray(float* array, size_t arrayLength, int numberOfMaxValues)
{
    std::deque<float> maxValues = std::deque<float>(numberOfMaxValues,0);
    
    for(int i = 0; i < arrayLength; i++)
    {
        if(maxValues.front() < fabsf(array[i]) )
        {
            maxValues.pop_front();
            maxValues.push_front(array[i]);
            std::sort(maxValues.begin(), maxValues.end());
        }
    }
    
    float averageMax = std::accumulate(maxValues.begin(), maxValues.end(), 0.0f) / (float)numberOfMaxValues;
    
    if(averageMax == 0.0f)
        averageMax = 1.0f; // prevent division by 0
    
    for(int i = 0; i < arrayLength; i++)
    {
        array[i] = array[i] > averageMax ? 1.0f : array[i] / averageMax;
        
        assert( array[i] != NAN );
    }
}

float getArrayMedian(float* array, size_t arrayLength)
{
    std::vector<float> allValues = std::vector<float>();
    allValues.assign(array, array + arrayLength);
    
    std::sort(allValues.begin(), allValues.end());
    
    return allValues[arrayLength/2];
}

float getArrayMean(float* array, size_t arrayLength)
{
    std::vector<float> allValues = std::vector<float>();
    allValues.assign(array, array + arrayLength);
    
    return std::accumulate(allValues.begin(), allValues.end(), 0.0f) / arrayLength;
}

std::vector<int> findMaxIndexes(const float* buffer, size_t length, int numberOfMaxValues)
{
	std::vector<float> copyOfBuffer(buffer,buffer+length);
    
    std::vector<int> maxIndexes = std::vector<int>(numberOfMaxValues, 0);
    
    for (int i = 0; i < numberOfMaxValues; i++)
    {
        int maxIndex = 0;
        
        for (int k = 0; k < length; k++)
        {
            if(copyOfBuffer[k] > copyOfBuffer[maxIndex])
                maxIndex = k;
        }
        
        copyOfBuffer[maxIndex] = 0.0f; //mark maximum with 0 so we can get next highest max
        maxIndexes[i] = maxIndex;
    }
    
    return maxIndexes;
}

std::vector<std::vector<int>> getClumpIndexes(std::vector<float> array, int minimumConsecutiveValues, float minimumValueToBeClump)
{
    std::vector<std::vector<int>> clumps = std::vector<std::vector<int>>();
    
    std::vector<int> currentClump = std::vector<int>();
    
    for(int i = 0; i < array.size(); i++)
    {
        if(array[i] >= minimumValueToBeClump)
        {
            currentClump.push_back(i);
        }
        else
        {
            if(currentClump.size() >= minimumConsecutiveValues)
            {
                clumps.push_back(currentClump);
                currentClump = std::vector<int>();
            }
            else
            {
                currentClump.empty();
            }
        }
    }
    
    return clumps;
}