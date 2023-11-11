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

#include "JuceHeader.h"

#include "Theme.h"

namespace showmidi
{
    class PaintedButton : public Button
    {
    public:
        PaintedButton();
        PaintedButton(const String&);
        
        void paintButton(Graphics&, bool, bool) override;

        void setBoundsForTouch(int, int, int, int);

        void drawName(Graphics&, Justification);
        void drawDrawable(Graphics&, Drawable&);
        
    private:
        void setBounds(Rectangle<int>);
        void setBounds(int, int, int, int);

        static constexpr int DEFAULT_TOUCH_OUTSET { 12 };
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaintedButton)
    };
}
