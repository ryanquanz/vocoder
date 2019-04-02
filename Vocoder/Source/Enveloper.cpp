//
//  Enveloper.cpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#include "Enveloper.hpp"

std::vector<float> Enveloper::envelope(const std::vector<float> &samples, int group_size, float cutoff_frequency) {
    std::vector<float> squared_samples;
    
    for (float sample : samples) {
        squared_samples.push_back(pow(sample, 2.0));
    }
    
    std::vector<float> downsampled_signal;
    
    for(int group_index = 0; group_index < samples.size(); group_index += group_size) {
        int samples_to_process = std::min(group_size, (int)samples.size() - group_index);
        float total_sample = 0;

        for(int sample_index = 0; sample_index < samples_to_process; sample_index++) {
            total_sample += squared_samples[group_index+sample_index];
        }

        for(int sample_index = 0; sample_index < samples_to_process; sample_index++) downsampled_signal.push_back(total_sample/samples_to_process);
    }
    
    ReferenceCountedArray<dsp::IIR::Coefficients<float>> coefficient_sets = dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(cutoff_frequency, 44100, 1);
    for(dsp::IIR::Coefficients<float> *coefficients : coefficient_sets) {
        dsp::IIR::Filter<float> filter(coefficients);
        float *audio[1] = { &downsampled_signal[0] };
        dsp::AudioBlock<float> block(audio, 1, (int)downsampled_signal.size());
        dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);
    }
    
    std::vector<float> output;
    for(float sample : downsampled_signal){
        output.push_back(sqrt(2 * sample));
    }
    
//    std::vector<float> enveloped_samples;
//
//    for(int group_index = 0; group_index < samples.size(); group_index += group_size) {
//        int samples_to_process = std::min(group_size, (int)samples.size() - group_index);
//        float max_sample = 0;
//
//        for(int sample_index = 0; sample_index < samples_to_process; sample_index++) {
//            float magnitude(fabsf(samples[group_index + sample_index]));
//            if(magnitude > max_sample) max_sample = magnitude;
//        }
//
//        for(int sample_index = 0; sample_index < samples_to_process; sample_index++) enveloped_samples.push_back(max_sample);
//    }
//
//    IIRCoefficients coefficients = IIRCoefficients::makeLowPass(44100, cutoff_frequency);
//    IIRFilter filter;
//    filter.setCoefficients(coefficients);
//    filter.processSamples(&enveloped_samples[0], (int)enveloped_samples.size());
    
    return output;
}
