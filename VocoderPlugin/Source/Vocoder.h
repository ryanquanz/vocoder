/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             Vocoder
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Vocoder audio plugin.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2017, linux_make

 type:             AudioProcessor
 mainClass:        Vocoder

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

#include "Enveloper.h"
#include "FilterBank.h"
#include "GainAdjuster.h"

//==============================================================================
class Vocoder  : public AudioProcessor
{
public:
    //==============================================================================
    Vocoder()
        : AudioProcessor (BusesProperties().withInput  ("Input",     AudioChannelSet::stereo())
                                           .withOutput ("Output",    AudioChannelSet::stereo())
                                           .withInput  ("Sidechain", AudioChannelSet::stereo()))
    {
        addParameter (freqbins = new AudioParameterFloat ("freqbins", "Frequency Bins TEST2", 0.0f, 1.0f, 0.5f));
    }

    ~Vocoder() {}

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        // the sidechain can take any layout, the main bus needs to be the same on the input and output
        return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet()
                 && ! layouts.getMainInputChannelSet().isDisabled();
    }

    //==============================================================================
    void prepareToPlay (double, int) override {}
    void releaseResources() override          {}

    void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override            { return new GenericAudioProcessorEditor (this); }
    bool hasEditor() const override                          { return true; }
    const String getName() const override                    { return "Vocoder"; }
    bool acceptsMidi() const override                        { return false; }
    bool producesMidi() const override                       { return false; }
    double getTailLengthSeconds() const override             { return 0.0; }
    int getNumPrograms() override                            { return 1; }
    int getCurrentProgram() override                         { return 0; }
    void setCurrentProgram (int) override                    {}
    const String getProgramName (int) override               { return {}; }
    void changeProgramName (int, const String&) override     {}
    bool isVST2() const noexcept                             { return (wrapperType == wrapperType_VST); }

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override
    {
        MemoryOutputStream stream (destData, true);

        stream.writeFloat (*freqbins);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);

        freqbins->setValueNotifyingHost (stream.readFloat());
    }

private:
    //==============================================================================
    AudioParameterFloat* freqbins;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Vocoder)
};
