/*
  ==============================================================================

    Enveloper.h
    Created: 2 Apr 2019 11:12:14am
    Author:  Austin Hardy

  ==============================================================================
*/

#pragma once

#include <stdio.h>
#include <algorithm>
#include "../JuceLibraryCode/JuceHeader.h"

class Enveloper {
public:
    std::vector<float> envelope(const std::vector<float> &, int, float);
};
