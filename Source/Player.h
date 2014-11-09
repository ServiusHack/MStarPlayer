#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Player : public Component {

public:
	virtual void setGain(float gain) = 0;

	virtual float getGain() = 0;

	virtual void setPan(float pan) = 0;

	virtual float getPan() = 0;

	virtual void setSoloMute(bool soloMute) = 0;

	virtual bool getSoloMute() = 0;

	virtual void setSolo(bool solo) = 0;

	virtual bool getSolo() = 0;

	virtual void setMute(bool mute) = 0;

	virtual bool getMute() = 0;
	
	virtual XmlElement* saveToXml() const = 0;

	virtual void setOutputChannels(int outputChannels) = 0;
};
