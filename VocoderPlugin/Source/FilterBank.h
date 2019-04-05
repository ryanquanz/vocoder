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
    float frequencyAt(int);
    int getNumFilters();
    void applyFilter(int, float *, int);
private:
    std::vector<IIRFilter> m_filters;
    std::vector<float> m_frequencies;
};
