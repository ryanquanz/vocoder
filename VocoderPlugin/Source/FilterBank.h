/*
  ==============================================================================

    FilterBank.h
    Created: 2 Apr 2019 11:32:43am
    Author:  Austin Hardy

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class FilterBank {
public:
    FilterBank(const std::vector<float> &, int, float);
    std::vector<std::vector<float>> applyFilters(const std::vector<float> &);
    std::vector<float> reconstruct(const std::vector<std::vector<float>> &);
private:
    std::vector<IIRFilter> m_filters;
};
