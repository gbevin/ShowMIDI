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
#include "PortListComponent.h"

#include "AboutComponent.h"
#include "MidiDeviceInfoComparator.h"
#include "MidiDevicesListener.h"
#include "PaintedButton.h"
#include "SettingsComponent.h"

namespace showmidi
{
    struct PortListComponent::Pimpl : public MouseListener, public MidiDevicesListener
    {
        Pimpl(PortListComponent* owner, SettingsManager& manager) :
            owner_(owner),
            manager_(manager)
        {
            owner_->addMouseListener(this, false);
            manager_.getMidiDevicesListeners().add(this);
        }
        
        ~Pimpl()
        {
            manager_.getMidiDevicesListeners().remove(this);
            owner_->removeMouseListener(this);
        }
        
        static constexpr int X_VISIBILITY = 10;
        static constexpr int Y_VISIBILITY = -2;
        static constexpr int X_PORT = 34;
        static constexpr int PORT_RIGHT_MARGIN = 8;
        static constexpr int PORT_Y_SPACING = 29;
        
        void paint(Graphics& g)
        {
            auto& settings = manager_.getSettings();
            auto& theme = settings.getTheme();
            g.setFont(theme.fontLabel());
            
            auto hidden_svg = hiddenSvg_->createCopy();
            hidden_svg->replaceColour(Colours::black, theme.colorLabel);
            auto visible_svg = visibleSvg_->createCopy();
            visible_svg->replaceColour(Colours::black, theme.colorData);
            
            int y_offset = 0;
            
            auto devices = MidiInput::getAvailableDevices();
            
            MidiDeviceInfoComparator comparator;
            devices.sort(comparator);
            {
                ScopedLock guard(midiDevicesLock_);
                midiDevices_ = devices;
            }
            
            for (int i = 0; i < devices.size(); ++i)
            {
                auto info = devices[i];
                if (settings.isMidiDeviceVisible(info.identifier))
                {
                    g.setColour(theme.colorData);
                    visible_svg->drawAt(g, X_VISIBILITY, (float)y_offset + Y_VISIBILITY, 1.0);
                }
                else
                {
                    g.setColour(theme.colorLabel);
                    hidden_svg->drawAt(g, X_VISIBILITY, (float)y_offset + Y_VISIBILITY, 1.0);
                }
                g.drawText(info.name,
                           X_PORT, y_offset,
                           owner_->getWidth() - X_PORT - PORT_RIGHT_MARGIN, theme.labelHeight(),
                           Justification::centredLeft, true);
                
                y_offset += PORT_Y_SPACING;
            }
            
            lastHeight_ = y_offset;
            
            owner_->setSize(owner_->getWidth(), lastHeight_);
        }
        
        void refreshMidiDevices()
        {
            if (owner_->isVisible())
            {
                owner_->repaint();
            }
        }
        
        void mouseUp(const MouseEvent& event)
        {
            if (event.mouseWasDraggedSinceMouseDown())
            {
                return;
            }
            
            ScopedLock g(midiDevicesLock_);

            int port = event.getMouseDownY() / PORT_Y_SPACING;
            if (port < midiDevices_.size())
            {
                auto port_info = midiDevices_[port];
                auto& settings = manager_.getSettings();
                settings.setMidiDeviceVisible(port_info.identifier, !settings.isMidiDeviceVisible(port_info.identifier));
                owner_->repaint();
            }
        }

        int getVisibleHeight() const
        {
            return lastHeight_;
        }

        PortListComponent* const owner_;
        SettingsManager& manager_;
        Array<MidiDeviceInfo> midiDevices_;
        CriticalSection midiDevicesLock_;

        std::unique_ptr<Drawable> hiddenSvg_ = Drawable::createFromImageData(BinaryData::hidden_svg, BinaryData::hidden_svgSize);
        std::unique_ptr<Drawable> visibleSvg_ = Drawable::createFromImageData(BinaryData::visible_svg, BinaryData::visible_svgSize);
        
        int lastHeight_ { 0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };
    
    PortListComponent::PortListComponent(SettingsManager& m) : pimpl_(new Pimpl(this, m)) {}
    PortListComponent::~PortListComponent() = default;
    
    int PortListComponent::getVisibleHeight() const   { return pimpl_->getVisibleHeight(); }

    void PortListComponent::paint(Graphics& g) { pimpl_->paint(g); }
}
