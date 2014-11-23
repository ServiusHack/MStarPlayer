#pragma once

#include <functional>

typedef std::function<void()> ChannelCountChangedCallback;

#include "../JuceLibraryCode/JuceHeader.h"

class MixerControlableChangeListener {
public:
	virtual void gainChanged(float /*gain*/) {};

	virtual void panChanged(float /*pan*/) {};

	virtual void soloChanged(bool /*solo*/) {};

	virtual void muteChanged(bool /*mute*/) {};
};

class MixerControlable {
public:
	virtual void setGain(float gain) = 0;

	virtual float getGain() const = 0;

	void addChangeListener(MixerControlableChangeListener* listener) {
		if (!m_listeners.contains(listener))
			m_listeners.add(listener);
	}

	void removeChangeListener(MixerControlableChangeListener* listener) {
		m_listeners.removeFirstMatchingValue(listener);
	}

	virtual void setPan(float pan) = 0;

	virtual float getPan() const = 0;

	virtual void setSoloMute(bool soloMute) = 0;

	virtual bool getSoloMute() const = 0;

	virtual void setSolo(bool solo) = 0;

	virtual bool getSolo() const = 0;

	virtual void setMute(bool mute) = 0;

	virtual bool getMute() const = 0;

	virtual float getVolume() const = 0;

protected:
	Array<MixerControlableChangeListener*> m_listeners;
};


class Player : public Component, public MixerControlable {

public:
	
	virtual XmlElement* saveToXml() const = 0;

	virtual void setOutputChannels(int outputChannels) = 0;

	virtual std::vector<MixerControlable*> getSubMixerControlables() = 0;

	virtual void SetChannelCountChangedCallback(ChannelCountChangedCallback callback) = 0;
};
