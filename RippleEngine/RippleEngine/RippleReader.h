//
//  RippleReader.h
//  RippleEngine
//
//  Created by Ahmed Jafri on 5/29/16.
//  Copyright © 2016 Ripple. All rights reserved.
//

#ifndef RippleReader_h
#define RippleReader_h

#include <vector>

class RippleReader {
public:
    //returns a float array of the song from a start index and length
    virtual std::vector<float> read(int start, int length) = 0;
    virtual int numberOfSamples() { return 0; };
    virtual long sampleRate() {return 44100; };
};

#endif /* RippleReader_h */
