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
#include "AboutComponent.h"

#include "PaintedButton.h"

namespace showmidi
{
    PaintedButton::PaintedButton() : Button({})
    {
    }
    
    PaintedButton::PaintedButton(const String& name) : Button(name)
    {
    }
    
    void PaintedButton::paintButton(Graphics&, bool, bool)
    {
        // no-op
    }

    void PaintedButton::setBoundsForTouch(int x, int y, int w, int h)
    {
        auto bounds = Rectangle<int> {x, y, w, h};
        Component::setBounds(bounds.expanded(DEFAULT_TOUCH_OUTSET));
    }

    void PaintedButton::drawName(Graphics& g, Justification justificationType)
    {
        auto bounds = getBounds().reduced(DEFAULT_TOUCH_OUTSET);
        g.drawText(getName(),
                   bounds.getX(), bounds.getY(),
                   bounds.getWidth(), bounds.getHeight(),
                   justificationType);
    }
    
    void PaintedButton::drawDrawable(Graphics& g, Drawable& drawable)
    {
        auto bounds = getBounds().reduced(DEFAULT_TOUCH_OUTSET);
        drawable.drawAt(g, (float)bounds.getX(), (float)bounds.getY(), 1.0f);
    }
}
