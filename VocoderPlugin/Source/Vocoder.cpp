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

#define MODULATOR_ARG 1
#define OUTPUT_ARG 2
#define CARRIER_ARG 3
#define SAMPLE_RATE 48000
#define BITS_PER_SAMPLE 16
#define N_BANDS 33
// Q is set to have filter bandwith to be 1/3 octave
#define Q_VALUE 4.318
#define ENVELOPE_SAMPLE_SIZE 10

vector<IIRFilter> buildFilters();

const float filter_frequencies[N_BANDS] = {12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};

vector<vector<float>> applyFilterBank(vector<float> input, vector<IIRFilter> filters, int num_samples);
vector<vector<float>> applyEnvelope(vector<vector<float>> inputVector);
vector<vector<float>> multiplyEnvelopes(vector<vector<float>> envelopes, vector<vector<float>> modulator);
vector<float> reconstruct(vector<vector<float>> inputs);

void Vocoder::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi) {
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto filters = buildFilters();
    
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
    
    vector<float> modulatorData(modulatorPointer, modulatorPointer + sizeof(float) * buffer.getNumSamples());
    vector<float> carrierData(carrierPointer, carrierPointer + sizeof(float) * buffer.getNumSamples());
    
    auto filteredModulatorData = applyFilterBank(modulatorData, filters, buffer.getNumSamples());
    auto filteredCarrierData = applyFilterBank(modulatorData, filters, buffer.getNumSamples());
    auto envelopedFilteredModulatorData = applyEnvelope(filteredModulatorData);
    auto adjustedModulator = multiplyEnvelopes(envelopedFilteredModulatorData, filteredCarrierData);
    auto output = reconstruct(adjustedModulator);
    
    copy(adjustedModulator[20].begin(), adjustedModulator[20].end(), modulatorPointer);
}

vector<float> reconstruct(vector<vector<float>> inputs) {
    vector<float> output(inputs[0].size(), 0);
    for(auto input : inputs) {
        for(int i = 0; i < output.size(); i++) {
            output[i] += input[i];
        }
    }
    float max_sample = 0;
    for(auto sample:output) {
        if(sample > max_sample) max_sample = sample;
    }
    for(int i = 0; i < output.size(); i++) output[i] /= max_sample;
    return output;
}

vector<vector<float>> multiplyEnvelopes(vector<vector<float>> envelopes, vector<vector<float>> modulator) {
    vector<vector<float>> outputs;
    
    for(int i = 0; i < envelopes.size(); i++) {
        auto envelope(envelopes[i]);
        auto modulator(envelopes[i]);
        vector<float> output;
        
        for(int j = 0; j < envelope.size(); j++) output.push_back(modulator[j] * envelope[j]);
        
        outputs.push_back(output);
    }
    
    return outputs;
}

vector<IIRFilter> buildFilters() {
    // Build filters
    vector<IIRFilter> band_pass_filters;
    
    for(int i = 0; i < N_BANDS; i++) {
        IIRCoefficients band_pass_coefficients = IIRCoefficients::makeBandPass(SAMPLE_RATE, filter_frequencies[i], Q_VALUE);
        IIRFilter band_pass_filter;
        band_pass_filter.setCoefficients(band_pass_coefficients);
        band_pass_filters.push_back(band_pass_filter);
    }
    
    return band_pass_filters;
}

vector<vector<float>> applyFilterBank(vector<float> input, vector<IIRFilter> filters, int num_samples) {
    vector<vector<float>> all_outputs;
    for(int i = 0; i < N_BANDS; i++) {
        vector<float> output = input;
        filters.at(i).processSamples(&output[0], num_samples);
        all_outputs.push_back(output);
    }
    
    return all_outputs;
}

vector<vector<float>> applyEnvelope(vector<vector<float>> inputVector) {
    vector<vector<float>> output;
    for (auto audioIter = inputVector.begin(); audioIter != inputVector.end(); ++audioIter) {
        vector<float> audio(*audioIter);
        vector<float> envelopedAudio;
        
        for(int audioIndex = 0; audioIndex < audio.size(); audioIndex += ENVELOPE_SAMPLE_SIZE) {
            int samplesToCheck = min(ENVELOPE_SAMPLE_SIZE, (int)(audio.size() - audioIndex));
            float maxValue = 0;
            
            for(int envelopeIndex = audioIndex; envelopeIndex < audioIndex + samplesToCheck; envelopeIndex++) {
                float magnitude = fabs(audio[envelopeIndex]);
                if(magnitude > maxValue) maxValue = magnitude;
            }
            
            for(int i = 0; i < ENVELOPE_SAMPLE_SIZE; i++) envelopedAudio.push_back(maxValue);
        }
        output.push_back(envelopedAudio);
    }
    
    return output;
}
