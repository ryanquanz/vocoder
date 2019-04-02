//
//  Main.cpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterBank.hpp"
#include "FileHandler.hpp"
#include "Enveloper.hpp"
#include "GainAdjuster.hpp"

#define MODULATOR_ARG 1
#define OUTPUT_ARG 2
#define CARRIER_ARG 3
#define SAMPLE_RATE 48000
#define BITS_PER_SAMPLE 16
#define N_BANDS 33
// Q is set to have filter bandwith to be 1/3 octave
#define Q_VALUE 4.318
#define ENVELOPE_SAMPLE_SIZE 10

const std::vector<float> FILTER_FREQUENCIES {12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};

int main (int argc, char* argv[])
{
    FileHandler file_handler;
    Enveloper enveloper;
    FilterBank filter_bank(FILTER_FREQUENCIES, SAMPLE_RATE, Q_VALUE);
    GainAdjuster gain_adjuster;
    
    std::vector<float> modulator_samples(file_handler.readSamplesFromFile(argv[MODULATOR_ARG]));
    std::vector<std::vector<float>> modulator_sample_bands = filter_bank.applyFilters(modulator_samples);
    
    std::vector<float> carrier_samples(file_handler.readSamplesFromFile(argv[CARRIER_ARG]));
    std::vector<std::vector<float>> carrier_sample_bands = filter_bank.applyFilters(carrier_samples);
    
    std::vector<std::vector<float>> enveloped_modulator_sample_bands;
    for(int i = 0; i < modulator_sample_bands.size(); i++) {
        enveloped_modulator_sample_bands.push_back(enveloper.envelope(modulator_sample_bands[i], ENVELOPE_SAMPLE_SIZE, FILTER_FREQUENCIES[i]/5));
    }
    
    std::vector<std::vector<float>> gain_adjusted_carrier_sample_bands;
    
    for(int band_index = 0; band_index < modulator_sample_bands.size(); band_index++) {
        gain_adjusted_carrier_sample_bands.push_back(
            gain_adjuster.adjust_gain(carrier_sample_bands[band_index], enveloped_modulator_sample_bands[band_index])
        );
    }
    
    std::vector<float> output(filter_bank.reconstruct(gain_adjusted_carrier_sample_bands));
    file_handler.writeSamplesToFile(output, "~/output.wav", SAMPLE_RATE, BITS_PER_SAMPLE);
    
    return 0;
}


