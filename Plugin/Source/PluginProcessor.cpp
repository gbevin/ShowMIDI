/*
 * This file is part of ShowMIDI.
 * Copyright (command) 2023 Uwyn LLC.  https://www.uwyn.com
 *
 * ShowMIDI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ShowMIDI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace showmidi
{
    ShowMIDIPluginAudioProcessor::ShowMIDIPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                      )
#endif
    {
    }
    
    ShowMIDIPluginAudioProcessor::~ShowMIDIPluginAudioProcessor()
    {
    }
    
    const juce::String ShowMIDIPluginAudioProcessor::getName() const
    {
        return JucePlugin_Name;
    }
    
    bool ShowMIDIPluginAudioProcessor::acceptsMidi() const
    {
        return true;
    }
    
    bool ShowMIDIPluginAudioProcessor::producesMidi() const
    {
        return true;
    }
    
    bool ShowMIDIPluginAudioProcessor::isMidiEffect() const
    {
        return true;
    }
    
    double ShowMIDIPluginAudioProcessor::getTailLengthSeconds() const
    {
        return 0.0;
    }
    
    int ShowMIDIPluginAudioProcessor::getNumPrograms()
    {
        return 1;
    }
    
    int ShowMIDIPluginAudioProcessor::getCurrentProgram()
    {
        return 0;
    }
    
    void ShowMIDIPluginAudioProcessor::setCurrentProgram(int index)
    {
    }
    
    const juce::String ShowMIDIPluginAudioProcessor::getProgramName(int index)
    {
        return {};
    }
    
    void ShowMIDIPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
    {
    }
    
    void ShowMIDIPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        // Use this method as the place to do any pre-playback
        // initialisation that you need..
    }
    
    void ShowMIDIPluginAudioProcessor::releaseResources()
    {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool ShowMIDIPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
    {
#if JucePlugin_IsMidiEffect
        juce::ignoreUnused (layouts);
        return true;
#else
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        // Some plugin hosts, such as certain GarageBand versions, will only
        // load plugins that support stereo bus layouts.
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
            && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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
    
    void ShowMIDIPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
    {
        juce::ScopedNoDenormals noDenormals;
        
        for (const MidiMessageMetadata metadata : midiMessages)
        {
            auto editor = (ShowMIDIPluginAudioProcessorEditor*)getActiveEditor();
            if (editor != nullptr)
            {
                editor->handleIncomingMidiMessage(metadata.getMessage());
            }
        }
    }
    
    bool ShowMIDIPluginAudioProcessor::hasEditor() const
    {
        return true;
    }
    
    juce::AudioProcessorEditor* ShowMIDIPluginAudioProcessor::createEditor()
    {
        return new ShowMIDIPluginAudioProcessorEditor(*this);
    }
    
    void ShowMIDIPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
    {
    }
    
    void ShowMIDIPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
    {
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new showmidi::ShowMIDIPluginAudioProcessor();
}
