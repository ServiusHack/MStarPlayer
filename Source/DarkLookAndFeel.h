#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppLookAndFeelInterface.h"

class DarkLookAndFeel
	: public LookAndFeel_V3
	, public AppLookAndFeel
		
{
public:
	DarkLookAndFeel();

	virtual AppLookAndFeel::Type getType() override;
};
