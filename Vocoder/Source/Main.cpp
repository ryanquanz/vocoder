#include "../JuceLibraryCode/JuceHeader.h"
using namespace std;

#define MODULATOR_ARG 1
#define OUTPUT_ARG 2
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 32
#define N_BANDS 33
// Q is set to have filter bandwith to be 1/3 octave
#define Q_VALUE 4.318
#define ENVELOPE_SAMPLE_SIZE 100

const float filter_frequencies[N_BANDS] = {12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};

void applyEnvelopes(vector<AudioBuffer<float>> &, vector<AudioBuffer<float>> &);
void applyFilterBank(unique_ptr<AudioBuffer<float>> &, vector<AudioBuffer<float>> &);
unique_ptr<AudioBuffer<float>> readFile(char *);
void writeFile(string, AudioBuffer<float>&);
//==============================================================================

int main (int argc, char* argv[])
{
    // Load modulator as AudioBuffer
    unique_ptr<AudioBuffer<float>> modulatorBuffer(readFile(argv[MODULATOR_ARG]));
    
    // Split modulator into frequency bands
    vector<AudioBuffer<float>> modulatorFilterOutput(N_BANDS, AudioBuffer<float>(1, modulatorBuffer->getNumSamples()));
    applyFilterBank(modulatorBuffer, modulatorFilterOutput);
    
    // Output modulator frequency bands for debugging
    for(int i = 0; i < N_BANDS; i++) {
        stringstream outputFile;
        outputFile << argv[OUTPUT_ARG] << filter_frequencies[i] << "FILTER_BANK.wav";
        writeFile(outputFile.str(), modulatorFilterOutput[i]);
    }
    
    // Create envelope for each modulator band
    vector<AudioBuffer<float>> envelopeOutput(N_BANDS, AudioBuffer<float>(1, modulatorBuffer->getNumSamples()));
    applyEnvelopes(modulatorFilterOutput, envelopeOutput);
    
    // Output envelopes for debugging
    for(int i = 0; i < N_BANDS; i++) {
        stringstream outputFile;
        outputFile << argv[OUTPUT_ARG] << filter_frequencies[i] << "ENVELOPE.wav";
        writeFile(outputFile.str(), envelopeOutput[i]);
    }
    
    // Load carrier as AudioBuffer
    unique_ptr<AudioBuffer<float>> carrierBuffer(readFile(argv[MODULATOR_ARG]));
    
    // Split carrier into frequency bands
    vector<AudioBuffer<float>> carrierFilterOutput(N_BANDS, AudioBuffer<float>(1, modulatorBuffer->getNumSamples()));
    applyFilterBank(carrierBuffer, carrierFilterOutput);

    // TODO Apply modulator envelopes to carrier bands (I think it's just element-by-element multiplication?)
    
    // TODO Combine enveloped carrier bands into single file
    
    // TODO Output and see if it makes robot sounds? (reuse writeFile)
}

/*****************
 * Some things are broken here!
 *
 * We need to put the output data through a low pass fixture to help smooth out the curves.
 *
 * I think the absolute value function is broken... Not sure why. Output wave has no 0s in it...
 *
 * This ignores the remainder of samples from file, i.e. the last 0-9 samples are ignored
 *
 *****************/
void applyEnvelopes(vector<AudioBuffer<float>> &inputBuffers, vector<AudioBuffer<float>> &outputBuffers) {
    for(int i = 0; i < N_BANDS; i++) {
        AudioBuffer<float> inputBuffer = inputBuffers[i];
        
        AudioBuffer<float> absInputBuffer(1, inputBuffer.getNumSamples());
        
        for(int j = 0; j < inputBuffer.getNumSamples(); j++) {
            // Use std::abs to avoid JUCE function
            absInputBuffer.addSample(0, j, std::abs(inputBuffer.getSample(0, j)));
        }
        
        // Use below code for debugging absolute value function
        // stringstream outputFile;
        // outputFile << "~/Desktop/output" << filter_frequencies[i] << "ABS.wav";
        // writeFile(outputFile.str(), absInputBuffer);
        
        for (int j = 0; j < absInputBuffer.getNumSamples()/ENVELOPE_SAMPLE_SIZE; j++) {
            float max_sample = 0;
            for(int k = 0; k < ENVELOPE_SAMPLE_SIZE; k++) {
                float sample = absInputBuffer.getSample(0, j*ENVELOPE_SAMPLE_SIZE + k);
                if(sample > max_sample){
                    max_sample = sample;
                }
            }
            for(int k = 0; k < ENVELOPE_SAMPLE_SIZE; k++) outputBuffers[i].addSample(0, j * ENVELOPE_SAMPLE_SIZE + k, max_sample);
        }
    }
}

void applyFilterBank(unique_ptr<AudioBuffer<float>> & buffer, vector<AudioBuffer<float>> & outputBuffers) {
    // Build filters
    vector<IIRFilter> band_pass_filters;
    
    for(int i = 0; i < N_BANDS; i++) {
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
    // Delete file to make sure that we are starting a new file
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
