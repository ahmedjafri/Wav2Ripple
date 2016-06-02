//
//  FileReader.cpp
//  Ripple
//
//  Created by Ahmed Jafri on 5/12/13.
//  Copyright (c) 2013 Ahmed Jafri. All rights reserved.
//

#include "FileReader.h"



FileReader::FileReader(){
    
}

FileReader::FileReader(const char * pathToPCMFile){
    this->file = pathToPCMFile;
}

std::vector<float> FileReader::read(int start, int length) {
    FILE* fileHandle = fopen(file,"r");

    float* output = (float*)malloc(sizeof(float)*length);
    
    fseek(fileHandle, start*sizeof(float) + sizeof(long long), SEEK_SET);
    
    if(fread(output, sizeof(float), length, fileHandle) < 1){
        return std::vector<float>(*output, length);
    }
    
    fclose(fileHandle);
    
    return std::vector<float>(*output, length);
}

long FileReader::sampleRate() {
    return 44100;
}

int FileReader::numberOfSamples() {
    FILE* fileHandle = fopen(file,"r");

    fseek(fileHandle, 0, SEEK_END);
    long fileSizeBytes = ftell(fileHandle) - sizeof(long long);
    
    fclose(fileHandle);
    
    return (int)fileSizeBytes/sizeof(float);
}