//
//  Analyzer.h
//  Ripple
//
//  Created by Ahmed Jafri on 11/24/12.
//  Copyright (c) 2012 Ahmed Jafri. All rights reserved.
//

#ifndef Ripple_RPAnalyzer_h
#define Ripple_RPAnalyzer_h

#include <string>
#include <map>
#include <vector>
#include <functional>
#include "../RippleReader.h"

typedef struct
{
    float* buffer;
    int length;
} GenericBuffer;

typedef struct
{
    std::vector<float> buffer;
    int number_of_bins_per_window;
    int number_of_windows;
} FrequencyBuffer;

typedef struct
{
    const char* pathToMediaFile;
    int numberOfWindows;
    float* energyBuffer;
    float* speedBuffer;
    float* pitchBuffer;
} RippleFileData;

typedef struct
{
    std::vector<unsigned char> data;
    int w;
    int h;
} ImageData;

class RPAnalyzer {
public:
    ~RPAnalyzer();
    RPAnalyzer(std::string pathToPCMFile, float lengthOfSong, int numberOfBinsPerWindow, int inWindowLength, std::function<void(const float)> callbackFunction);
    //int fullSongLengthTotalSamples;
    FrequencyBuffer frequencyBinsPerWindowBuffer();
    std::vector<float> convertToNormalizedEnergy(const int frequencyBinMin, const int frequencyBinMax);
    std::vector<float> convertToIntensity();
    std::vector<bool> getObstacles();
    float* convertToSpeed();
    GenericBuffer getFrequencyPercentages();
    int mNumberOfBinsPerWindow;

    void flushToFile(std::string pathToRippleFile);
    RippleFileData readRippleFile(std::string pathToRippleFile);
    int numberOfWindows();
    ImageData getImageData();
    void writeFrequencyToImage(const std::string& fileName);
    
private:
    void init();
    void convertToFreqDomain();
    std::function<void(const float)> callbackFunction;
    std::vector<float> frequencyBinsBuffer;
    float* speedBuffer;
    std::vector<unsigned char> img;
    
    std::vector<float> pitches;
    RippleReader* reader;
    
    int windowLength;
    int windowOverlap;
    int mNumberOfWindows;
    float* file_buffer;
    std::string pathToPCMFile;
    int getSmallestBase2Length(int actualLength);
    float* readPCMData(int start, int length, float* returnData, bool useHammingWindow);
    bool isBetween(const float inX, const float inMin, const float inMax);
};

#endif
