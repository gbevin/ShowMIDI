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
#include "MainComponent.h"

#include "MidiDeviceComponent.h"

namespace showmidi
{
    class MidiDeviceInfoComparator
    {
    public:
        static int compareElements (MidiDeviceInfo first, MidiDeviceInfo second)
        {
            return (first.name < second.name) ? -1 : ((second.name < first.name) ? 1 : 0);
        }
    };

    struct MainComponent::Pimpl : public MultiTimer
    {
        static constexpr int MIN_MIDI_DEVICES_AUTO_SHOWN = 1;
        static constexpr int MAX_MIDI_DEVICES_AUTO_SHOWN = 6;
        static constexpr int MIDI_DEVICE_SPACING = 4;
        
        enum Timers
        {
            RenderDevices = 1,
            RefreshMidiDevices
        };
        
        Pimpl(MainComponent* owner) : owner_(owner)
        {
            refreshMidiDevices();
            
            // 30Hz
            startTimer(RenderDevices, 1000 / 30);
            
            // 5Hz
            startTimer(RefreshMidiDevices, 1000 / 5);
        }
        
        ~Pimpl()
        {
            stopTimer(RenderDevices);
            stopTimer(RefreshMidiDevices);
            
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                owner_->removeChildComponent(i.getValue());
                delete i.getValue();
            }
        }
        
        void paint(juce::Graphics& g)
        {
            g.fillAll(owner_->getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
        }
        
        void timerCallback(int timerID)
        {
            switch (timerID)
            {
                case RenderDevices:
                {
                    renderDevices();
                    break;
                }
                    
                case RefreshMidiDevices:
                {
                    refreshMidiDevices();
                    break;
                }
            }
        }
        
        void renderDevices()
        {
            auto height = owner_->getParentHeight();
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                auto c = i.getValue();
                c->render();
                height = std::max(height, c->getVisibleHeight());
            }
            
            auto width = std::max(owner_->getParentWidth(), midiDevices_.size() * (MidiDeviceComponent::getStandardWidth() + MIDI_DEVICE_SPACING) - MIDI_DEVICE_SPACING);
            owner_->setSize(width, height);

            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                i.getValue()->setSize(MidiDeviceComponent::getStandardWidth(), height);
            }
        }
        
        void refreshMidiDevices()
        {
            auto devices = MidiInput::getAvailableDevices();
            
            MidiDeviceInfoComparator comparator;
            devices.sort(comparator);
            
            // detect the previous devices that have now disappeared
            Array<String> devices_to_remove;
            for (HashMap<const String, MidiDeviceComponent*>::Iterator i(midiDevices_); i.next();)
            {
                auto identifier = i.getKey();
                bool found = false;
                for (int i = 0; i < devices.size(); ++i)
                {
                    if (devices[i].identifier == identifier)
                    {
                        found = true;
                        break;
                    }
                }
                
                if (!found)
                {
                    devices_to_remove.add(identifier);
                }
            }
            
            // check if all the devices are already present
            bool new_devices_preset = false;
            for (int i = 0; i < devices.size(); ++i)
            {
                if (!midiDevices_.contains(devices[i].identifier))
                {
                    new_devices_preset = true;
                    break;
                }
            }
            
            if (devices_to_remove.size() > 0 || new_devices_preset)
            {
                owner_->removeAllChildren();
                
                // remove the devices that disappeared
                for (int i = 0; i < devices_to_remove.size(); ++i)
                {
                    auto identifier = devices_to_remove[i];
                    auto component = midiDevices_.getReference(identifier);
                    delete component;
                    
                    midiDevices_.remove(identifier);
                }
                
                // create the new devices and reuse the existing ones
                // lay them out in alpabetical order
                for (int i = 0; i < devices.size(); ++i)
                {
                    auto info = devices[i];
                    if (!devices_to_remove.contains(info.identifier))
                    {
                        MidiDeviceComponent* component = midiDevices_.getReference(info.identifier);
                        if (component == nullptr)
                        {
                            component = new MidiDeviceComponent(info);
                            midiDevices_.set(info.identifier, component);
                        }
                        
                        component->setBounds(MIDI_DEVICE_SPACING + i * (component->getStandardWidth() + MIDI_DEVICE_SPACING), 0, component->getStandardWidth(), owner_->getParentHeight());
                        
                        owner_->addAndMakeVisible(component);
                    }
                }
                
                MessageManager::callAsync([this] () {
                    // resize the window in order to display the MIDI devices
                    auto viewport = owner_->getParentComponent();
                    if (viewport != nullptr)
                    {
                        auto window = viewport->getParentComponent();
                        if (window != nullptr)
                        {
                            auto window_width = (MidiDeviceComponent::getStandardWidth() + MIDI_DEVICE_SPACING) * std::max(MIN_MIDI_DEVICES_AUTO_SHOWN, std::min(MAX_MIDI_DEVICES_AUTO_SHOWN, midiDevices_.size())) + MIDI_DEVICE_SPACING;
                            window->setSize(window_width, window->getHeight());
                        }
                    }
                });
            }
        }
        
        MainComponent* const owner_;
        HashMap<const String, MidiDeviceComponent*> midiDevices_;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    MainComponent::MainComponent() : pimpl_(new Pimpl(this)) {}
    MainComponent::~MainComponent() = default;
    
    void MainComponent::paint(juce::Graphics& g) { pimpl_->paint(g); }
}
