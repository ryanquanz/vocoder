/*
  ==============================================================================

    GainAdjuster.cpp
    Created: 2 Apr 2019 11:32:54am
    Author:  Austin Hardy

  ==============================================================================
*/

#include "GainAdjuster.h"

void GainAdjuster::adjust_gain(float *samples, float *gain_adjustments, int numSamples) {
    for(int i = 0; i < numSamples; i++) {
        samples[i] = samples[i] * gain_adjustments[i];
    }
}
