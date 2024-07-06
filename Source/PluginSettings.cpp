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
#include "PluginSettings.h"
#include "PropertiesSettings.h"

namespace showmidi
{
    PluginSettings::PluginSettings()
    {
        PropertiesSettings properties_settings;
        settings_.setProperty(PropertiesSettings::VISUALIZATION, properties_settings.getVisualization(), nullptr);
        settings_.setProperty(PropertiesSettings::OCTAVE_MIDDLE_C, properties_settings.getOctaveMiddleC(), nullptr);
        settings_.setProperty(PropertiesSettings::NOTE_FORMAT, properties_settings.getNoteFormat(), nullptr);
        settings_.setProperty(PropertiesSettings::NUMBER_FORMAT, properties_settings.getNumberFormat(), nullptr);
        settings_.setProperty(PropertiesSettings::TIMEOUT_DELAY, properties_settings.getTimeoutDelay(), nullptr);
        
        theme_ = properties_settings.getTheme();
        settings_.setProperty(PropertiesSettings::THEME, theme_.generateXml(), nullptr);
    }
    
    PluginSettings::~PluginSettings()
    {
    }
    
    Visualization PluginSettings::getVisualization()
    {
        return (Visualization)(int)settings_.getProperty(PropertiesSettings::VISUALIZATION, PropertiesSettings::DEFAULT_VISUALIZATION);
    }
    
    void PluginSettings::setVisualization(Visualization visualization)
    {
        settings_.setProperty(PropertiesSettings::VISUALIZATION, visualization, nullptr);
    }

    int PluginSettings::getOctaveMiddleC()
    {
        return settings_.getProperty(PropertiesSettings::OCTAVE_MIDDLE_C, PropertiesSettings::DEFAULT_OCTAVE_MIDDLE_C);
    }
    
    void PluginSettings::setOctaveMiddleC(int octave)
    {
        settings_.setProperty(PropertiesSettings::OCTAVE_MIDDLE_C, octave, nullptr);
    }
    
    NoteFormat PluginSettings::getNoteFormat()
    {
        return (NoteFormat)(int)settings_.getProperty(PropertiesSettings::NOTE_FORMAT, PropertiesSettings::DEFAULT_NOTE_FORMAT);
    }
    
    void PluginSettings::setNoteFormat(NoteFormat format)
    {
        settings_.setProperty(PropertiesSettings::NOTE_FORMAT, format, nullptr);
    }
    
    NumberFormat PluginSettings::getNumberFormat()
    {
        return (NumberFormat)(int)settings_.getProperty(PropertiesSettings::NUMBER_FORMAT, PropertiesSettings::DEFAULT_NUMBER_FORMAT);
    }
    
    void PluginSettings::setNumberFormat(NumberFormat format)
    {
        settings_.setProperty(PropertiesSettings::NUMBER_FORMAT, format, nullptr);
    }
    
    int PluginSettings::getTimeoutDelay()
    {
        return settings_.getProperty(PropertiesSettings::TIMEOUT_DELAY, PropertiesSettings::DEFAULT_TIMEOUT_DELAY);
    }
    
    void PluginSettings::setTimeoutDelay(int delay)
    {
        settings_.setProperty(PropertiesSettings::TIMEOUT_DELAY, delay, nullptr);
    }
    
    WindowPosition PluginSettings::getWindowPosition()
    {
        return (WindowPosition)(int)settings_.getProperty(PropertiesSettings::WINDOW_POSITION, PropertiesSettings::DEFAULT_WINDOW_POSITION);
    }
    
    void PluginSettings::setWindowPosition(WindowPosition position)
    {
        settings_.setProperty(PropertiesSettings::WINDOW_POSITION, position, nullptr);
    }
    
    int PluginSettings::getControlGraphHeight()
    {
        return settings_.getProperty(PropertiesSettings::CONTROL_GRAPH_HEIGHT, PropertiesSettings::DEFAULT_CONTROL_GRAPH_HEIGHT);
    }
    
    void PluginSettings::setControlGraphHeight(int height)
    {
        settings_.setProperty(PropertiesSettings::CONTROL_GRAPH_HEIGHT, height, nullptr);
    }

    Theme& PluginSettings::getTheme()
    {
        return theme_;
    }
    
    void PluginSettings::storeTheme()
    {
        settings_.setProperty(PropertiesSettings::THEME, theme_.generateXml(), nullptr);
    }
    
    bool PluginSettings::isMidiDeviceVisible(const String&)
    {
        return true;
    }
    
    void PluginSettings::setMidiDeviceVisible(const String&, bool)
    {
        // no-op
    }

    ValueTree& PluginSettings::getValueTree()
    {
        return settings_;
    }
    
    void PluginSettings::copyValueTree(ValueTree& other)
    {
        settings_.copyPropertiesAndChildrenFrom(other, nullptr);
        theme_.parseXml(settings_.getProperty(PropertiesSettings::THEME, "").toString());
    }
}
