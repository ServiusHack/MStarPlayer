#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DefaultLookAndFeel
	: public LookAndFeel_V3
{
public:
	void drawProgressBar (Graphics& g, ProgressBar& progressBar,
						  int width, int height,
						  double progress, const String& textToShow);
};
