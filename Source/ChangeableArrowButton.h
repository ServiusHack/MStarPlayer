#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

class ChangeableArrowButton : public juce::Button
{
public:
    /** Creates an ArrowButton where the direction can be changed.

        @param buttonName       the name to give the button
        @param arrowDirection   the direction the arrow should point in, where 0.0 is
                                pointing right, 0.25 is down, 0.5 is left, 0.75 is up
        @param arrowColour      the colour to use for the arrow
    */
    ChangeableArrowButton(const juce::String& buttonName, float arrowDirection, juce::Colour arrowColour);

    /** @internal */
    void paintButton(juce::Graphics&, bool isMouseOverButton, bool isButtonDown) override;

    void setArrowDirection(float arrowDirection);

private:
    juce::Colour colour;
    juce::Path path;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeableArrowButton)
};
