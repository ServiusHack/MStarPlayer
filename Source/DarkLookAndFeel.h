#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DarkLookAndFeel
	: public LookAndFeel_V3
{
public:
	DarkLookAndFeel();

	void drawProgressBar (Graphics& g, ProgressBar& progressBar,
						  int width, int height,
						  double progress, const String& textToShow);
};
