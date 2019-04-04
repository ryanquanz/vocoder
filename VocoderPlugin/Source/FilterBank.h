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
    FilterBank(float, int);
    std::vector<std::vector<float>> applyFilters(const std::vector<float> &);
    std::vector<float> reconstruct(const std::vector<std::vector<float>> &);
    float frequencyAt(int);
private:
    std::vector<IIRFilter> m_filters;
    std::vector<float> m_frequencies;
};
