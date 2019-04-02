/*
  ==============================================================================

    GainAdjuster.h
    Created: 2 Apr 2019 11:32:54am
    Author:  Austin Hardy

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class GainAdjuster {
public:
    std::vector<float> adjust_gain(const std::vector<float>&, const std::vector<float>&);
};
