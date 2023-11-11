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
    {
    }
    
    ShowMIDIPluginAudioProcessor::~ShowMIDIPluginAudioProcessor()
    {
    }
    
    const String ShowMIDIPluginAudioProcessor::getName() const
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
    
    void ShowMIDIPluginAudioProcessor::setCurrentProgram(int)
    {
    }
    
    const String ShowMIDIPluginAudioProcessor::getProgramName(int)
    {
        return {};
    }
    
    bool ShowMIDIPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout&) const
    {
        return true;
    }

    void ShowMIDIPluginAudioProcessor::changeProgramName(int, const String&)
    {
    }

    void ShowMIDIPluginAudioProcessor::prepareToPlay(double, int)
    {
    }

    void ShowMIDIPluginAudioProcessor::releaseResources()
    {
    }

    void ShowMIDIPluginAudioProcessor::processBlock(AudioBuffer<float>&, MidiBuffer& midiMessages)
    {
        ScopedNoDenormals noDenormals;
        
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
    
    AudioProcessorEditor* ShowMIDIPluginAudioProcessor::createEditor()
    {
        return new ShowMIDIPluginAudioProcessorEditor(*this);
    }
    
    void ShowMIDIPluginAudioProcessor::getStateInformation(MemoryBlock&)
    {
    }
    
    void ShowMIDIPluginAudioProcessor::setStateInformation(const void*, int)
    {
    }
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new showmidi::ShowMIDIPluginAudioProcessor();
}
