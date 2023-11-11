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
#include "StoredSettings.h"

namespace showmidi
{
    const String StoredSettings::OCTAVE_MIDDLE_C = {"octaveMiddleC"};
    const String StoredSettings::TIMEOUT_DELAY = {"timeoutDelay"};
    const String StoredSettings::THEME = {"theme"};

    StoredSettings::StoredSettings()
    {
        reload();
    }
    
    StoredSettings::~StoredSettings()
    {
        flush();
    }
    
    int StoredSettings::getOctaveMiddleC()
    {
        return getGlobalProperties().getIntValue(OCTAVE_MIDDLE_C, DEFAULT_OCTAVE_MIDDLE_C);
    }
    
    void StoredSettings::setOctaveMiddleC(int octave)
    {
        getGlobalProperties().setValue(OCTAVE_MIDDLE_C, octave);
        flush();
    }
    
    int StoredSettings::getTimeoutDelay()
    {
        return getGlobalProperties().getIntValue(TIMEOUT_DELAY, DEFAULT_TIMEOUT_DELAY);
    }
    
    void StoredSettings::setTimeoutDelay(int delay)
    {
        getGlobalProperties().setValue(TIMEOUT_DELAY, delay);
        flush();
    }

    Theme& StoredSettings::getTheme()
    {
        return theme_;
    }
    
    void StoredSettings::storeTheme()
    {
        getGlobalProperties().setValue(THEME, theme_.generateXml());
    }

    PropertiesFile& StoredSettings::getGlobalProperties()
    {
        return *propertyFile_;
    }
    
    static PropertiesFile* createPropsFile(const String& filename)
    {
        PropertiesFile::Options options;
        options.applicationName = filename;
        options.filenameSuffix = "settings";
        options.osxLibrarySubFolder = "Application Support";
#if JUCE_LINUX
        options.folderName = String(".")+String(ProjectInfo::projectName).toLowerCase();
#else
        options.folderName = ProjectInfo::projectName;
#endif
        
        return new PropertiesFile(options);
    }
    
    void StoredSettings::flush()
    {
        propertyFile_->saveIfNeeded();
    }
    
    void StoredSettings::reload()
    {
        propertyFile_.reset(createPropsFile(ProjectInfo::projectName));
        
        auto& props = getGlobalProperties();
        if (!props.containsKey(THEME))
        {
            theme_ = Desktop::getInstance().isDarkModeActive() ? THEME_DARK : THEME_LIGHT;
        }
        else
        {
            auto theme_xml = props.getValue(THEME);
            Theme theme;
            theme.parseXml(theme_xml);
            theme_ = theme;
        }
    }
}
