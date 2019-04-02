/*
  ==============================================================================

    FilterBank.cpp
    Created: 2 Apr 2019 11:32:43am
    Author:  Austin Hardy

  ==============================================================================
*/

#include "FilterBank.h"

FilterBank::FilterBank(const std::vector<float> &frequencies, int sample_rate, float q_value) {
    for(float frequency : frequencies) {
        IIRCoefficients coefficients = IIRCoefficients::makeBandPass(sample_rate, frequency, q_value);
        IIRFilter filter;
        filter.setCoefficients(coefficients);
        m_filters.push_back(filter);
    }
}

std::vector<std::vector<float>> FilterBank::applyFilters(const std::vector<float> &samples) {
    std::vector<std::vector<float>> filtered_samples;
    
    for(IIRFilter filter : m_filters) {
        std::vector<float> samples_copy(samples);
        filter.processSamples(&samples_copy[0], (int)samples_copy.size());
        filtered_samples.push_back(samples_copy);
    }
    
    return filtered_samples;
}


std::vector<float> FilterBank::reconstruct(const std::vector<std::vector<float>> &sample_bands) {
    std::vector<float> output(sample_bands[0].size(), 0);
    
    for(std::vector<float> sample_band : sample_bands) {
        for(int sample_index = 0; sample_index < sample_bands[0].size(); sample_index++) {
            output[sample_index] =  output[sample_index]+ sample_band[sample_index];
        }
    }
    return output;
}
