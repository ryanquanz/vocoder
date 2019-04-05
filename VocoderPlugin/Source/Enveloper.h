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
    void envelope(float *, int, double, int, float);
};
