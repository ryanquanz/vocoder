//
//  FilterBank.hpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#ifndef FilterBank_hpp
#define FilterBank_hpp

#include "../JuceLibraryCode/JuceHeader.h"

class FilterBank {
public:
    FilterBank(const std::vector<float> &, int, float);
    std::vector<std::vector<float>> applyFilters(const std::vector<float> &);
    std::vector<float> reconstruct(const std::vector<std::vector<float>> &);
private:
    std::vector<IIRFilter> m_filters;
};

#endif /* FilterBank_hpp */
