//
//  FileReader.h
//  Ripple
//
//  Created by Ahmed Jafri on 5/12/13.
//  Copyright (c) 2013 Ahmed Jafri. All rights reserved.
//

#ifndef __Ripple__FileReader__
#define __Ripple__FileReader__

#include <iostream>
#include <vector>
#include "RippleReader.h"

class FileReader : public RippleReader {
public:
    //returns a float array of the song from a start index and length
    std::vector<float> read(int start, int length) override;
    int numberOfSamples() override;
    FileReader(const char * pathToPCMFile);
    FileReader();
    long sampleRate() override;
private:
    const char * file;
};

#endif /* defined(__Ripple__FileReader__) */
