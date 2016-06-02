//
//  RPCommonDSPFunctions.h
//  Ripple
//
//  Created by Ahmed Jafri on 1/20/15.
//  Copyright (c) 2015 Ripple. All rights reserved.
//

#ifndef __Ripple__RPCommonDSPFunctions__
#define __Ripple__RPCommonDSPFunctions__

#include <stdio.h>
#include <vector>

void normalizeArray(float* array, size_t arrayLength, int numberOfMaxValues);

float getArrayMedian(float* array, size_t arrayLength);
float getArrayMean(float* array, size_t arrayLength);

bool isBetween(const float inX, const float inMin, const float inMax);
std::vector<int> findMaxIndexes(const float* buffer, size_t length, int numberOfMaxValues);

std::vector<std::vector<int>> getClumpIndexes(std::vector<float> array, int minimumConsecutiveValues, float minimumValueToBeClump);

#endif /* defined(__Ripple__RPCommonDSPFunctions__) */