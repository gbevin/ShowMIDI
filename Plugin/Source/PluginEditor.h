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
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

namespace showmidi
{
    class ShowMIDIPluginAudioProcessorEditor : public juce::AudioProcessorEditor
    {
    public:
        ShowMIDIPluginAudioProcessorEditor(ShowMIDIPluginAudioProcessor&);
        ~ShowMIDIPluginAudioProcessorEditor() override;
        
        void handleIncomingMidiMessage(const MidiMessage&);

        void paint(juce::Graphics&) override;
        void resized() override;
        
        struct Pimpl;
    private:
        std::unique_ptr<Pimpl> pimpl_ { nullptr };
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowMIDIPluginAudioProcessorEditor)
    };
}
