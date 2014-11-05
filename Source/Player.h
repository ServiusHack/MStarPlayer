#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Player : public Component {

public:
	virtual void setGain(float gain) = 0;
	
	virtual XmlElement* saveToXml() const = 0;

	virtual void setOutputChannels(int outputChannels) = 0;
};
