/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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

//==============================================================================
VocoderPluginAudioProcessor::VocoderPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Modulator",  AudioChannelSet::mono(), true)
                       .withInput  ("Carrier",  AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

VocoderPluginAudioProcessor::~VocoderPluginAudioProcessor()
{
}

//==============================================================================
const String VocoderPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VocoderPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VocoderPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VocoderPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VocoderPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocoderPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VocoderPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocoderPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String VocoderPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void VocoderPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VocoderPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VocoderPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocoderPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void VocoderPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
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
    
    auto modulatorPointer = buffer.getWritePointer(0);
    auto carrierPointer = buffer.getWritePointer(1);
    
    vector<float> modulatorData(modulatorPointer, modulatorPointer + sizeof(float) * buffer.getNumSamples());
    vector<float> carrierData(carrierPointer, carrierPointer + sizeof(float) * buffer.getNumSamples());
    
    auto filteredModulatorData = applyFilterBank(modulatorData, filters, buffer.getNumSamples());
    auto filteredCarrierData = applyFilterBank(modulatorData, filters, buffer.getNumSamples());
    auto envelopedFilteredCarrierData = applyEnvelope(filteredCarrierData);
    auto adjustedModulator = multiplyEnvelopes(envelopedFilteredCarrierData, filteredModulatorData);
    
    copy(adjustedModulator[10].begin(), adjustedModulator[10].end(), modulatorPointer);
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

//==============================================================================
bool VocoderPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VocoderPluginAudioProcessor::createEditor()
{
    return new VocoderPluginAudioProcessorEditor (*this);
}

//==============================================================================
void VocoderPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VocoderPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocoderPluginAudioProcessor();
}
