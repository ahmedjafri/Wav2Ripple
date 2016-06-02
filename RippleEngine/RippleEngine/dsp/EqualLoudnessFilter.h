//
//  EqualLoudnessParameters.h
//  Ripple
//
//  Created by Ahmed Jafri on 12/13/13.
//  Copyright (c) 2013 Ahmed Jafri. All rights reserved.
//

#ifndef Ripple_EqualLoudnessFilter_h
#define Ripple_EqualLoudnessFilter_h

#define EQL_LENGTH 31

const float f [] = {0, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 24000};

const float af [] = {0.55, 0.532, 0.506, 0.480, 0.455, 0.432, 0.409, 0.387, 0.367, 0.349, 0.330, 0.315, 0.301, 0.288, 0.276, 0.267, 0.259, 0.253, 0.250, 0.246, 0.244, 0.243, 0.243, 0.243, 0.242, 0.242, 0.245, 0.254, 0.271, 0.301, 0.55};

const float Lu [] = {-32, -31.6, -27.2, -23.0, -19.1, -15.9, -13.0, -10.3, -8.1, -6.2, -4.5, -3.1, -2.0, -1.1, -0.4, 0.0, 0.3, 0.5, 0.0, -2.7, -4.1, -1.0, 1.7, 2.5, 1.2, -2.1, -7.1, -11.2, -10.7, -3.1, -5};

const float Tf [] = {80, 78.5, 68.7, 59.5, 51.1, 44.0, 37.5, 31.5, 26.5, 22.1, 17.9, 14.4, 11.4, 8.6, 6.2, 4.4, 3.0, 2.2, 2.4, 3.5, 1.7, -1.3, -4.2, -6.0, -5.4, -1.5, 6.0, 12.6, 13.9, 12.3, 10.0};

const float EqualLoudness [] = {0.147095526032617, 0.162499748179070, 0.242420523825701, 0.317910894017423, 0.388635206322049, 0.450362062317068, 0.509901563284114, 0.567623104396514, 0.618662254189766, 0.665227795446596, 0.712265839392622, 0.751583048489977, 0.786727724285946, 0.819477579786365, 0.848621178941572, 0.870231763269301, 0.888208535102040, 0.900082459866020, 0.900003811764372, 0.887378535794805, 0.895708695452076, 0.927255557759746, 0.955492204063849, 0.969595832502964, 0.962039433337780, 0.925417045375273, 0.856517307099695, 0.790158666756024, 0.765693624688023, 0.776730820903273, 0.173558113702124};

class EqualLoudnessFilter
{
public:
    static float getEqualLoudnessFilterAtFrequency(const float inFrequency)
    {
        int k = 0;
        for (int i = 0; i < EQL_LENGTH; i++) {
            if(f[i] <= inFrequency){
                k = i;
            }
        }
        
        return EqualLoudness[k] + (EqualLoudness[k+1] - EqualLoudness[k])*( (inFrequency - f[k])/(f[k+1] - f[k]) );
    }

private:

};

#endif
