#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

/**
        Default look and feel which is JUCE's look and feel with some minor improvements.
*/
class DefaultLookAndFeel : public juce::LookAndFeel_V3
{
public:
    DefaultLookAndFeel();

    virtual void drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar, int width, int height,
        double progress, const juce::String& textToShow) override;

    void drawLevelMeter(juce::Graphics&, int width, int height, float level, float peak);
};
