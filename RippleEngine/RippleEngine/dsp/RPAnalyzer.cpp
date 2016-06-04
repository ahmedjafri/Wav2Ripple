//
//  Analyze.c
//  Ripple
//
//  Created by Ahmed Jafri on 11/24/12.
//  Copyright (c) 2012 Ahmed Jafri. All rights reserved.
//

#include <stdio.h>
#include "../Globals.h"
#include "RPAnalyzer.h"
#include "EqualLoudnessFilter.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <deque>
#include <vector>
#include "TimeToFrequencyTransformer.h"
#include "RPCommonDSPFunctions.h"
#include "dywapitchtrack.h"
#include "../FileReader.h"

#define windowTime .05
#define percentOverlap 50.0f
#define LOOKAHEAD_WINDOWS 50

#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>
#include <cassert>
#include <stdio.h>

#ifdef __APPLE__

#elif __linux__
#include <sys/stat.h>
#else
#include <Windows.h>
#include <tchar.h>
#endif

#if defined(__APPLE__)
static bool fileExists(const char* filePath)
{
	if (access(filePath, F_OK) != -1) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
#elif defined(__linux__)
static bool fileExists(const char *filename) {
    struct stat st;
    int result = stat(filename, &st);
    return result == 0;
}
#else
int fileExists(TCHAR * file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file, &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		//FindClose(&handle); this will crash
		FindClose(handle);
	}
	return found;
}
#endif


static void writeFloatVectorToTextFile(const std::string& fileName, const std::vector<float>& values)
{
    std::ostringstream stream;
    std::copy(values.begin(), values.end(), std::ostream_iterator<float>(stream,","));
    std::string str = stream.str();
    std::ofstream file(fileName.c_str());
    file << str;
}

RPAnalyzer::RPAnalyzer(std::string inPathToPCMFile, float lengthOfSong, int numberOfBinsPerWindow, int inWindowLength, std::function<void(const float)> inCallbackFunction):
callbackFunction(inCallbackFunction),
mNumberOfBinsPerWindow(numberOfBinsPerWindow),
windowLength(inWindowLength*2.0),
pitches(std::vector<float>(0))
{
    pathToPCMFile = inPathToPCMFile;
    reader = new FileReader(inPathToPCMFile.c_str());
    RPAnalyzer(*reader,numberOfBinsPerWindow,inWindowLength,inCallbackFunction);
}

RPAnalyzer::RPAnalyzer(RippleReader& inReader, int numberOfBinsPerWindow, int inWindowLength, std::function<void(const float)> inCallbackFunction):
callbackFunction(inCallbackFunction),
mNumberOfBinsPerWindow(numberOfBinsPerWindow),
windowLength(inWindowLength*2.0),
pitches(std::vector<float>(0))
{
    reader = &inReader;
#if defined(__APPLE__) || defined(__linux__)
    const char *rawFilePathToPCM = pathToPCMFile.c_str();
#else
    TCHAR* rawFilePathToPCM = (TCHAR*)pathToPCMFile.c_str();
#endif
    
    if(!fileExists(rawFilePathToPCM))
    {
        throw std::runtime_error("PCM file does not exist.");
    }
    
    windowOverlap = windowLength*(percentOverlap/100.0f);
    mNumberOfWindows = reader->numberOfSamples()/(windowLength - windowOverlap);
    
    frequencyBinsBuffer = std::vector<float>(inWindowLength * mNumberOfWindows, 0.0f);
    speedBuffer = (float *)calloc(mNumberOfWindows, sizeof(float));
    pitches = std::vector<float>(mNumberOfWindows, 0.0f);
    convertToFreqDomain();
}

RPAnalyzer::~RPAnalyzer()
{
    free(speedBuffer);
}

void RPAnalyzer::convertToFreqDomain()
{        
    std::vector<float> windowedOutput(windowLength);
    
    //Create a dywapitchtracker Object
    dywapitchtracker pitchtracker;
    //Initialise the object with this function
    dywapitch_inittracking(&pitchtracker);
    TimeToFrequencyTransformer transformer = TimeToFrequencyTransformer(windowedOutput.data(), windowLength);

    for(int k = 0; k < mNumberOfWindows; k++)
    {
        int starting = k*(windowLength - windowOverlap);
        readPCMData(starting, windowLength, windowedOutput.data(), true);
        
        transformer.assignNewTimeSignal(windowedOutput.data());
        transformer.windowed = false;
        std::vector<float> band = transformer.getFrequencyBand();
        
        assert(!isnan(band[0]));
        
        memcpy(&frequencyBinsBuffer[k*windowLength/2], band.data(), (windowLength/2)*sizeof(float));
        
        if(callbackFunction != 0)
        {
            callbackFunction((float)k/(float)mNumberOfWindows);
        }
    }
    
    if(callbackFunction != 0)
    {
        callbackFunction(1.0f); // done analyzing
    }
}

int rows = 10;

ImageData RPAnalyzer::getImageData()
{
    int w = mNumberOfWindows;
    int h = windowLength/2;
    
    if(img.size() == 0)
    {
        img = std::vector<unsigned char>(3*w*h);

        for(int i=0; i<w; i++)
        {
            int x=i;
            
            float* currentWindowFrequencies = &frequencyBinsBuffer[i*h];

            for(int j=0; j<h; j++)
            {
                int y=(h-1)-j;
                unsigned char r = 255 - (currentWindowFrequencies[j]*255);
                unsigned char g = 255 - (currentWindowFrequencies[j]*255);
                unsigned char b = 255 - (currentWindowFrequencies[j]*255);
                if (r > 255) r=255;
                if (g > 255) g=255;
                if (b > 255) b=255;
                img[(x+y*w)*3+2] = (unsigned char)(r);
                img[(x+y*w)*3+1] = (unsigned char)(g);
                img[(x+y*w)*3+0] = (unsigned char)(b);
            }
        }
    }

    ImageData returnData;
    returnData.data = img;
    returnData.w = w;
    returnData.h = h;
    return returnData;
}

void RPAnalyzer::writeFrequencyToImage(const std::string& fileName)
{
    ImageData img = getImageData();
    
    FILE *f;
    int filesize = 54 + 3*img.w*img.h;  //w is your image width, h is image height, both int
    int w = img.w, h = img.h;
    
    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};
    
    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);
    
    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);
    
    f = fopen(fileName.c_str(),"wb");
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    for(int i=0; i<h; i++)
    {
        fwrite(img.data.data() + (w*(h-i-1)*3),3,w,f);
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }
    fclose(f);
}

FrequencyBuffer RPAnalyzer::frequencyBinsPerWindowBuffer()
{
    std::vector<float> buffer = std::vector<float>(mNumberOfWindows*mNumberOfBinsPerWindow,0.0f);
    
    for(int k = 0; k < mNumberOfWindows; k++)
    {
        int samplesPerBin = (windowLength/2) / mNumberOfBinsPerWindow;
        int binIter = 0;
        float currentBinValue = 0;
        
        float* currentWindowFrequencies = &frequencyBinsBuffer[k*windowLength/2];
        
        for(int i = 0; i < (windowLength/2); i++)
        {
            if( (i/samplesPerBin) > binIter && binIter < mNumberOfBinsPerWindow)
            {
                assert(currentBinValue != NAN); // make sure we are not dividing by 0

                buffer[k*mNumberOfBinsPerWindow + binIter] = currentBinValue;
                binIter++;
                currentBinValue = 0;
            }

            currentBinValue += currentWindowFrequencies[i];
        }
    }
    
    //normalizeWithLookAheads();
    
    FrequencyBuffer freqBuffer;
    freqBuffer.buffer = buffer;
    freqBuffer.number_of_bins_per_window = mNumberOfBinsPerWindow;
    freqBuffer.number_of_windows = mNumberOfWindows;
    
    return freqBuffer;
}


std::vector<float> RPAnalyzer::convertToNormalizedEnergy(const int frequencyBinMin, const int frequencyBinMax)
{
    std::vector<float> returnEnergyBuffer = std::vector<float>(mNumberOfWindows);
    
    for(int currentWindow = 0; currentWindow < mNumberOfWindows; currentWindow++)
    {
        float currentEnergyValue = 0;
        float* currentWindowFrequencies = &frequencyBinsBuffer[currentWindow*windowLength/2];

        for(int i = frequencyBinMin; i < (frequencyBinMax-1); i++)
        {
            currentEnergyValue += currentWindowFrequencies[i];
        }
        
        assert(currentEnergyValue != NAN); // make sure we are not dividing by 0

        returnEnergyBuffer[currentWindow] = currentEnergyValue;
    }

    normalizeArray(returnEnergyBuffer.data(), mNumberOfWindows, 100);
    
    return returnEnergyBuffer;
}

static int calculateLookAheadWindowSize(int targetWindowSize, int currentWindow, int totalWindows)
{
    int lookAheadWindows = targetWindowSize;
    
    if(currentWindow < targetWindowSize)
    {
        lookAheadWindows = currentWindow;
    }
    else if(currentWindow > (totalWindows - targetWindowSize))
    {
        lookAheadWindows = totalWindows - currentWindow;
    }
    
    return lookAheadWindows;
}

std::vector<float> RPAnalyzer::convertToIntensity()
{
    std::vector<float> normalizedEnergyBuffer = this->convertToNormalizedEnergy(1, 10);
    
    std::vector<float> energyBuffer = std::vector<float>(mNumberOfWindows);
    
    for (int currentWindow = 0; currentWindow < mNumberOfWindows; currentWindow++)
    {
        float lookAheadSum = 0.0f;
        
        int lookAheadWindows = calculateLookAheadWindowSize(LOOKAHEAD_WINDOWS, currentWindow, mNumberOfWindows);
        
        for(int j = 0; j < lookAheadWindows; j++)
        {
            lookAheadSum += normalizedEnergyBuffer[currentWindow-j] / (float)((j+1)*1.5f); // windows before
            lookAheadSum += normalizedEnergyBuffer[currentWindow+j] / (float)((j+1)*1.5f); // windows after
        }
        
        lookAheadWindows = lookAheadWindows == 0 ? 1 : lookAheadWindows; // prevent division by 0
        
        energyBuffer[currentWindow] = lookAheadSum / (float)(lookAheadWindows * 2.0f);
        
        assert(!isnan(energyBuffer[currentWindow])); // make sure we are not dividing by 0
    }
    
    normalizeArray(energyBuffer.data(), mNumberOfWindows, 10);
    
    assert(!isnan(energyBuffer[0]));
    
#if DEBUG
    writeFloatVectorToTextFile(std::string("intensities.txt"), energyBuffer);
#endif
    
    return energyBuffer;
}

std::vector<bool> RPAnalyzer::getObstacles()
{
    std::vector<float> normalizedEnergyBuffer = this->convertToNormalizedEnergy(1, 10);
    std::vector<bool> obstacles = std::vector<bool>(mNumberOfWindows);
    
    for(int i = 15; i < normalizedEnergyBuffer.size(); i++)
    {
        std::vector<int> maxIndexes = findMaxIndexes(&normalizedEnergyBuffer[i-15], 15, 3);
        
        if(std::find(maxIndexes.begin(), maxIndexes.end(), 14) != maxIndexes.end()) {
            obstacles[i] = true;
        }
    }
    
    return obstacles;
}

#define RIPPLE_MEDIALOCATOR_LENGTH 512

void RPAnalyzer::flushToFile(std::string pathToRippleFile)
{
    if(pathToRippleFile.length() >= RIPPLE_MEDIALOCATOR_LENGTH)
    {
        throw std::runtime_error("Media locator URL is too long.");
    }
    
    FILE* fp = fopen(pathToRippleFile.c_str(), "w");
    int32_t mNumberOfWindows32Bit = mNumberOfWindows;
    fwrite(&mNumberOfWindows32Bit, sizeof(int32_t), 1, fp);
    fwrite(pathToRippleFile.c_str(),sizeof(char),RIPPLE_MEDIALOCATOR_LENGTH,fp);
    
}

RippleFileData readRippleFile(std::string pathToRippleFile)
{
    RippleFileData fileData;
    
    FILE* fp = fopen(pathToRippleFile.c_str(), "r");
    
    return fileData;
}

// PRIVATE METHODS

int RPAnalyzer::getSmallestBase2Length(int actualLength)
{
    int test_length = 2;
    
    while (true)
    {
        test_length*=2;
        if(test_length >= actualLength)
        {
            actualLength = test_length;
            break;
        }
    }
    
    return actualLength;
}

float* RPAnalyzer::readPCMData(int start, int length, float* returnData, bool useHammingWindow)
{
    std::vector<float> pcmData = reader->read(start, length);
    
    //Hamming window parameters
    float a = 25.0f/46.0f;
    float b = 21.0f/46.0f;
    
    float rollingAverage = 0;
    
    // find average value
    for(int n = 0; n < length; n++)
    {
        rollingAverage+= pcmData[n];
    }
    
    float DCoffset = rollingAverage / length;
    
    for(int n = 0; n < length; n++){
        float window = useHammingWindow ? a - b*cosf( ( 2*M_PI*n ) / ( length - 1 ) ) : 1.0f;
        returnData[n] = window * (pcmData[n] - DCoffset);
    }
    
    return returnData;
}

int RPAnalyzer::numberOfWindows()
{
    return mNumberOfWindows;
}

bool RPAnalyzer::isBetween(const float inX, const float inMin, const float inMax) {
    return (inMin <= inX && inX < inMax);
}

