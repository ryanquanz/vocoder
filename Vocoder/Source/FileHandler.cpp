//
//  FileHandler.cpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#include "FileHandler.hpp"

std::vector<float> FileHandler::readSamplesFromFile(const std::string &path) {
    File input_file(path);
    AudioFormatManager format_manager;
    format_manager.registerBasicFormats();
    
    std::unique_ptr<AudioFormatReader> formatReader(format_manager.createReaderFor(input_file));
    std::unique_ptr<AudioBuffer<float>> buffer(new AudioBuffer<float>(1, (int)formatReader->lengthInSamples));
    formatReader->read(buffer.get(), 0, buffer->getNumSamples(), 0, false, false);
    
    std::vector<float> samples;
    for(int i = 0; i < buffer->getNumSamples(); i++) samples.push_back(buffer->getSample(0, i));
    
    return samples;
}

void FileHandler::writeSamplesToFile(std::vector<float> &samples, const std::string &path, int sample_rate, int bits_per_sample) {
    File output_file(path);
    // Delete file to make sure that we are starting a new file
    output_file.deleteFile();
    
    // This has to be a normal ptr because the writer deletes it for us.
    OutputStream *outputStream(output_file.createOutputStream());
    
    WavAudioFormat wav;
    std::unique_ptr<AudioFormatWriter> writer(wav.createWriterFor(outputStream, sample_rate, 1, bits_per_sample, StringPairArray(), 0));

    float *outputSamples[1] = {&samples[0]};
    writer->writeFromFloatArrays(outputSamples, 1, (int)samples.size());
}

