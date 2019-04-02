//
//  GainAdjuster.cpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#include "GainAdjuster.hpp"


std::vector<float> GainAdjuster::adjust_gain(const std::vector<float> &samples, const std::vector<float> &gain_adjustments) {
    std::vector<float> gain_adjusted_samples;
    for(int sample_index = 0; sample_index < gain_adjustments.size(); sample_index++) {
        gain_adjusted_samples.push_back(samples[sample_index] * gain_adjustments[sample_index]);
    }
    return gain_adjusted_samples;
}
