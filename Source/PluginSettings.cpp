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
        settings_.setProperty(PropertiesSettings::OCTAVE_MIDDLE_C, properties_settings.getOctaveMiddleC(), nullptr);
        settings_.setProperty(PropertiesSettings::TIMEOUT_DELAY, properties_settings.getTimeoutDelay(), nullptr);
        
        theme_ = properties_settings.getTheme();
        settings_.setProperty(PropertiesSettings::THEME, theme_.generateXml(), nullptr);
    }
    
    PluginSettings::~PluginSettings()
    {
    }
    
    int PluginSettings::getOctaveMiddleC()
    {
        return settings_.getProperty(PropertiesSettings::OCTAVE_MIDDLE_C, PropertiesSettings::DEFAULT_OCTAVE_MIDDLE_C);
    }
    
    void PluginSettings::setOctaveMiddleC(int octave)
    {
        settings_.setProperty(PropertiesSettings::OCTAVE_MIDDLE_C, octave, nullptr);
    }
    
    int PluginSettings::getTimeoutDelay()
    {
        return settings_.getProperty(PropertiesSettings::TIMEOUT_DELAY, PropertiesSettings::DEFAULT_TIMEOUT_DELAY);
    }
    
    void PluginSettings::setTimeoutDelay(int delay)
    {
        settings_.setProperty(PropertiesSettings::TIMEOUT_DELAY, delay, nullptr);
    }
    
    Theme& PluginSettings::getTheme()
    {
        return theme_;
    }
    
    void PluginSettings::storeTheme()
    {
        settings_.setProperty(PropertiesSettings::THEME, theme_.generateXml(), nullptr);
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
