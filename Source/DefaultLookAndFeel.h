#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DefaultLookAndFeel : public LookAndFeel_V3
{
public:
	virtual void drawProgressBar(Graphics& g, ProgressBar& progressBar,
								  int width, int height,
								  double progress, const String& textToShow) override;

    virtual void drawLevelMeter(Graphics&, int width, int height, float level) override;
};
