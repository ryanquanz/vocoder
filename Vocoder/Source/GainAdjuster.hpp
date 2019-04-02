//
//  GainAdjuster.hpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#ifndef GainAdjuster_hpp
#define GainAdjuster_hpp
#include "../JuceLibraryCode/JuceHeader.h"

class GainAdjuster {
public:
    std::vector<float> adjust_gain(const std::vector<float>&, const std::vector<float>&);
};


#endif /* GainAdjuster_hpp */
