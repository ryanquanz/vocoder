/*
  ==============================================================================

    Enveloper.cpp
    Created: 2 Apr 2019 11:12:14am
    Author:  Austin Hardy

  ==============================================================================
*/

#include "Enveloper.h"

void Enveloper::envelope(float *samples, int numSamples, double sampleRate, int group_size, float cutoff_frequency) {
    for(int i = 0; i < numSamples; i++ ) {
        samples[i] = pow(samples[i], 2.0);
    }
    
    for(int group_index = 0; group_index < numSamples; group_index += group_size) {
        int samples_to_process = std::min(group_size, numSamples - group_index);
        float total_sample = 0;
        
        for(int sample_index = 0; sample_index < samples_to_process; sample_index++) {
            total_sample += samples[group_index+sample_index];
        }
        
        for(int sample_index = 0; sample_index < samples_to_process; sample_index++) {
            samples[group_index+sample_index] = total_sample/samples_to_process;
        }
    }
    
    ReferenceCountedArray<dsp::IIR::Coefficients<float>> coefficient_sets = dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(cutoff_frequency, sampleRate, 1);
    
    for(dsp::IIR::Coefficients<float> *coefficients : coefficient_sets) {
        dsp::IIR::Filter<float> filter(coefficients);
        float *audio[1] = { &samples[0] };
        dsp::AudioBlock<float> block(audio, 1, numSamples);
        dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);
    }
    
    for(int i = 0; i < numSamples; i++ ) {
        samples[i] = sqrt(2 * samples[i]);
    }
}
