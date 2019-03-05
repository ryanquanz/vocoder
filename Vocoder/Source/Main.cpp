#include "../JuceLibraryCode/JuceHeader.h"
using namespace std;

#define INPUT_ARG 1
#define OUTPUT_ARG 2
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 32
#define N_BANDS 33
#define Q_VALUE 4.318
const float filter_frequencies[N_BANDS] = {12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};

void applyFilterBank(unique_ptr<AudioBuffer<float>> &, vector<AudioBuffer<float>> &);
unique_ptr<AudioBuffer<float>> readFile(char *);
void writeFile(string, AudioBuffer<float>&);
//==============================================================================
int main (int argc, char* argv[])
{
    unique_ptr<AudioBuffer<float>> inputBuffer = readFile(argv[INPUT_ARG]);
    vector<AudioBuffer<float>> filterOutput(N_BANDS, AudioBuffer<float>(1, inputBuffer->getNumSamples()));

    applyFilterBank(inputBuffer, filterOutput);
    
    
    for(int i = 0; i < N_BANDS; i++) {
        stringstream outputFile;
        outputFile << argv[OUTPUT_ARG] << filter_frequencies[i] << ".wav";
        writeFile(outputFile.str(), filterOutput[i]);
    }
}

void applyFilterBank(unique_ptr<AudioBuffer<float>> & buffer, vector<AudioBuffer<float>> & outputBuffers) {
    // Build filters
    vector<IIRFilter> band_pass_filters;
    
    for(int i = 0; i < N_BANDS; i++) {
        cout << filter_frequencies[i] << endl;
        IIRCoefficients band_pass_coefficients = IIRCoefficients::makeBandPass(SAMPLE_RATE, filter_frequencies[i], Q_VALUE);
        IIRFilter band_pass_filter;
        band_pass_filter.setCoefficients(band_pass_coefficients);
        band_pass_filters.push_back(band_pass_filter);
    }
    
    // Run filters
    vector<float> samples;
    for(int j = 0; j < buffer->getNumSamples(); j++)  samples.push_back(buffer->getSample(0, j));

    for(int i = 0; i < N_BANDS; i++) {
        vector<float> band_pass_samples = samples;
        band_pass_filters.at(i).processSamples(&band_pass_samples[0], buffer->getNumSamples());
        
        for(int j = 0; j < buffer->getNumSamples(); j++) {
            outputBuffers[i].addSample(0, j, band_pass_samples[j]);
        }
    }
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

void writeFile(string path, AudioBuffer<float>& buffer) {
    File outputFile(path);
    // Delete file to make sure that we are not appending
    outputFile.deleteFile();

    // This has to be a normal ptr because the writer deletes it for us.
    OutputStream *outputStream(
        outputFile.createOutputStream()
    );
    
    WavAudioFormat wav;
    unique_ptr<AudioFormatWriter> writer(
        wav.createWriterFor(outputStream, SAMPLE_RATE, 1, BITS_PER_SAMPLE, StringPairArray(), 0)
    );
    
    writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
}
