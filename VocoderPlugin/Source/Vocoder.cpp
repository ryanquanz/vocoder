/*
  ==============================================================================

    Vocoder.cpp
    Created: 2 Apr 2019 11:19:29am
    Author:  Austin Hardy

  ==============================================================================
*/

#include "Vocoder.h"

#include <vector>

using namespace std;

#define BITS_PER_SAMPLE 16
#define N_BANDS 33
#define Q_VALUE 4.318               // Q is set to have filter bandwith to be 1/3 octave
#define ENVELOPE_SAMPLE_SIZE 10

vector<IIRFilter> buildFilters();

const vector<float> FILTER_FREQUENCIES {12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};

void Vocoder::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi) {
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    auto mainInputOutput = getBusBuffer (buffer, true, 0);
    auto sideChainInput  = getBusBuffer (buffer, true, 1);
    auto modulatorPointer = mainInputOutput.getWritePointer(0);
    auto carrierPointer = sideChainInput.getWritePointer(0);
    
    // Take the middle filter frequencies for now
    int skip = *freqbins / N_BANDS;
    vector<float> filt_freqs;
    for (int i = 0; i < N_BANDS; i++) {
        if ( i % skip == 0 ) filt_freqs.push_back(FILTER_FREQUENCIES[i]);
    }

    Enveloper enveloper;
    FilterBank filter_bank(filt_freqs, getSampleRate(), Q_VALUE);
    GainAdjuster gain_adjuster;
    
    std::vector<float> modulator_samples(modulatorPointer, modulatorPointer + sizeof(float) * buffer.getNumSamples());
    std::vector<std::vector<float>> modulator_sample_bands = filter_bank.applyFilters(modulator_samples);
    
    std::vector<float> carrier_samples(carrierPointer, carrierPointer + sizeof(float) * buffer.getNumSamples());
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
    copy(output.begin(), output.end(), modulatorPointer);
}
