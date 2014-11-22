#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppLookAndFeelInterface.h"

class DefaultLookAndFeel
	: public LookAndFeel_V3
	, public AppLookAndFeel

{
public:
	virtual AppLookAndFeel::Type getType() override;
};
