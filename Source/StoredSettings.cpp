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
    const String StoredSettings::THEME = {"theme"};
    
    StoredSettings::StoredSettings()
    {
        reload();
    }
    
    StoredSettings::~StoredSettings()
    {
        flush();
    }
    
    Theme StoredSettings::loadTheme()
    {
        auto& props = getGlobalProperties();
        if (!props.containsKey(THEME))
        {
            return Desktop::getInstance().isDarkModeActive() ? THEME_DARK : THEME_LIGHT;
        }
        
        auto theme_xml = props.getValue(THEME);
        Theme theme;
        theme.parseXml(theme_xml);
        
        return theme;
    }
    
    void StoredSettings::storeTheme(Theme& theme)
    {
        getGlobalProperties().setValue(THEME, theme.generateXml());
    }

    PropertiesFile &StoredSettings::getGlobalProperties()
    {
        return *propertyFile_;
    }
    
    static PropertiesFile *createPropsFile(const String &filename)
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
    }
}
