//
//  Enveloper.hpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#ifndef Enveloper_hpp
#define Enveloper_hpp

#include <stdio.h>
#include <algorithm>
#include "../JuceLibraryCode/JuceHeader.h"

class Enveloper {
public:
    std::vector<float> envelope(const std::vector<float> &, int, float);
};

#endif /* Enveloper_hpp */
