/*
  ==============================================================================

    FilterBank.cpp
    Created: 2 Apr 2019 11:32:43am
    Author:  Austin Hardy

  ==============================================================================
*/

#include "FilterBank.h"

FilterBank::FilterBank(float frequency_band_size, int sample_rate) {
    float q_value = pow(2.0, frequency_band_size/2.0) / (pow(2.0, frequency_band_size) - 1);
    for(float center_frequency = 20 ; center_frequency < 20000 ; center_frequency *= pow(2.0, frequency_band_size)) {
        IIRCoefficients coefficients = IIRCoefficients::makeBandPass(sample_rate, center_frequency, q_value);
        IIRFilter filter;
        filter.setCoefficients(coefficients);
        m_filters.push_back(filter);
        m_frequencies.push_back(center_frequency);
    }
}

float FilterBank::frequencyAt(int i) {
    return m_frequencies[i];
}

int FilterBank::getNumFilters() {
    return int(m_filters.size());
}

void FilterBank::applyFilter(int filter, float *samples, int numSamples) {
    m_filters[filter].processSamples(samples, numSamples);
}
