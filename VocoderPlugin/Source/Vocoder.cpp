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
    auto modulatorPointer = mainInputOutput.getReadPointer(0);
    auto carrierPointer = sideChainInput.getReadPointer(0);
    
    int numSamples = buffer.getNumSamples();

    FilterBank filter_bank(*filter_band_size, getSampleRate());
    
    float output[numSamples];
    for(int i = 0; i < numSamples; i++) {
        output[i] = 0.0;
    }
    
    float modulator_signal[numSamples];
    float carrier_signal[numSamples];
    for(int j = 0; j < filter_bank.getNumFilters(); j++) {
        for(int i = 0; i < numSamples; i++) {
            modulator_signal[i] = modulatorPointer[i];
            carrier_signal[i] = carrierPointer[i];
        }
        
        filter_bank.applyFilter(j, modulator_signal, numSamples);
        filter_bank.applyFilter(j, carrier_signal, numSamples);

        enveloper.envelope(modulator_signal, numSamples, getSampleRate(), ENVELOPE_SAMPLE_SIZE, filter_bank.frequencyAt(j)/5);

        gain_adjuster.adjust_gain(carrier_signal, modulator_signal, numSamples);

        // Reconstruct
        for(int i = 0; i < numSamples; i++) {
            output[i] += carrier_signal[i];
        }
    }
    
    // Normalize output?
    
    float *mainOutputLeft = mainInputOutput.getWritePointer(0);
    float *mainOutputRight = mainInputOutput.getWritePointer(1);
    for(int i = 0; i < numSamples; i++) {
        mainOutputLeft[i] = output[i];
        mainOutputRight[i] = output[i];
    }
}
