/*
  ==============================================================================

    ButtonLookAndFeel.h

    ### Customized buttons design ###

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ColourPalette.h"

class ButtonLookAndFeel : public LookAndFeel_V4
{
public:
    void drawButtonBackground(
        Graphics& g, 
        Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        float borderRadius = 10.0f;

        if (shouldDrawButtonAsHighlighted)
			g.setColour(ColourPalette::primaryColour); // hover color
        else
			g.setColour(ColourPalette::btnColour); // normal color

        g.fillRoundedRectangle(bounds, borderRadius);

        // Set border colour
        g.setColour(ColourPalette::tertiaryColour.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, borderRadius, 2.0f);
    }
};
