/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
using namespace std;

#define INPUT_ARG 1
#define OUTPUT_ARG 2

unique_ptr<AudioBuffer<float>> readFile(char *);
void writeFile(char *, unique_ptr<AudioBuffer<float>>&);
//==============================================================================
int main (int argc, char* argv[])
{
    
    unique_ptr<AudioBuffer<float>> inputBuffer = readFile(argv[INPUT_ARG]);

    unique_ptr<AudioBuffer<float>> outputBuffer(new AudioBuffer<float>(1, inputBuffer->getNumSamples()));
    
    for(int i = 0; i < inputBuffer->getNumSamples(); i++) {
        outputBuffer->addSample(0, i, inputBuffer->getSample(0, i));
    }
    
    writeFile(argv[OUTPUT_ARG], outputBuffer);
}

unique_ptr<AudioBuffer<float>> readFile(char *path) {
    File inputFile(path);
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    unique_ptr<AudioFormatReader> formatReader(
        formatManager.createReaderFor(inputFile)
    );

    unique_ptr<AudioBuffer<float>> buffer(
        new AudioBuffer<float>(1, (int)formatReader->lengthInSamples)
    );
    
    formatReader->read(buffer.get(), 0, buffer->getNumSamples(), 0, false, false);
    
    return buffer;
}

void writeFile(char *path, unique_ptr<AudioBuffer<float>> & buffer) {
    File outputFile(path);
    // Delete file to make sure that we are not appending
    outputFile.deleteFile();

    // This has to be a normal ptr because the writer deletes it for us.
    OutputStream *outputStream(
        outputFile.createOutputStream()
    );
    
    WavAudioFormat wav;
    unique_ptr<AudioFormatWriter> writer(
        wav.createWriterFor(outputStream, 44100, 1, 32, StringPairArray(), 0)
    );
    
    writer->writeFromAudioSampleBuffer(*buffer.get(), 0, buffer->getNumSamples());
}
