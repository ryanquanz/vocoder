/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define MODULATOR_ARG 1
#define OUTPUT_ARG 2
#define CARRIER_ARG 3
#define SAMPLE_RATE 48000
#define BITS_PER_SAMPLE 16
#define N_BANDS 33
// Q is set to have filter bandwith to be 1/3 octave
#define Q_VALUE 4.318
#define ENVELOPE_SAMPLE_SIZE 10

using namespace std;

vector<IIRFilter> buildFilters();

const float filter_frequencies[N_BANDS] = {12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};

vector<vector<float>> applyFilterBank(vector<float> input, vector<IIRFilter> filters, int num_samples);
vector<vector<float>> applyEnvelope(vector<vector<float>> inputVector);
vector<vector<float>> multiplyEnvelopes(vector<vector<float>> envelopes, vector<vector<float>> modulator);

//==============================================================================
VocoderTake2AudioProcessor::VocoderTake2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Modulator",  AudioChannelSet::mono(), true)
                       .withInput  ("Carrier",  AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
}

VocoderTake2AudioProcessor::~VocoderTake2AudioProcessor()
{
}

//==============================================================================
const String VocoderTake2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VocoderTake2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VocoderTake2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VocoderTake2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VocoderTake2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocoderTake2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VocoderTake2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocoderTake2AudioProcessor::setCurrentProgram (int index)
{
}

const String VocoderTake2AudioProcessor::getProgramName (int index)
{
    return {};
}

void VocoderTake2AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VocoderTake2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VocoderTake2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocoderTake2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VocoderTake2AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
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
    auto modulatorPointer = buffer.getWritePointer(0);
    auto carrierPointer = buffer.getWritePointer(1);
    auto filters = buildFilters();
    
    vector<float> modulatorData(modulatorPointer, modulatorPointer + sizeof(float) * buffer.getNumSamples());
//    vector<float> carrierData(carrierPointer, carrierPointer + sizeof(float) * buffer.getNumSamples());
//
//    auto filteredModulatorData = applyFilterBank(modulatorData, filters, buffer.getNumSamples());
//    auto filteredCarrierData = applyFilterBank(modulatorData, filters, buffer.getNumSamples());
//    
//    copy(filteredCarrierData[10].begin(), filteredCarrierData[10].end(), modulatorPointer);
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


//==============================================================================
bool VocoderTake2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VocoderTake2AudioProcessor::createEditor()
{
    return new VocoderTake2AudioProcessorEditor (*this);
}

//==============================================================================
void VocoderTake2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VocoderTake2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocoderTake2AudioProcessor();
}
