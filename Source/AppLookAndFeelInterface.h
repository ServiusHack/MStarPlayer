#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class AppLookAndFeel
{
public:
	enum class Type {
		Default,
		Dark
	};

	virtual Type getType() = 0;
};