#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
	Default look and feel which is JUCE's look and feel with some minor improvements.
*/
class DefaultLookAndFeel : public LookAndFeel_V3
{
public:
	virtual void drawProgressBar(Graphics& g, ProgressBar& progressBar,
								  int width, int height,
								  double progress, const String& textToShow) override;

    virtual void drawLevelMeter(Graphics&, int width, int height, float level) override;
};
