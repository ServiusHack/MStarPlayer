#pragma once

#include <set>

#include "../JuceLibraryCode/JuceHeader.h"

class MixerControlableChangeListener {
public:
	virtual void gainChanged(float /*gain*/) {};

	virtual void panChanged(float /*pan*/) {};

	virtual void soloChanged(bool /*solo*/) {};

	virtual void muteChanged(bool /*mute*/) {};

	virtual void nameChanged(String /*name*/) {};
};

class MixerControlable {
public:
	virtual void setGain(float gain) = 0;

	virtual float getGain() const = 0;

	virtual void setPan(float pan) = 0;

	virtual float getPan() const = 0;

	virtual void setSoloMute(bool soloMute) = 0;

	virtual bool getSoloMute() const = 0;

	virtual void setSolo(bool solo) = 0;

	virtual bool getSolo() const = 0;

	virtual void setMute(bool mute) = 0;

	virtual bool getMute() const = 0;

	virtual float getVolume() const = 0;

// manage listeners
public:
	void addChangeListener(MixerControlableChangeListener* listener) {
		m_listeners.insert(listener);
	}
	void removeChangeListener(MixerControlableChangeListener* listener) {
		m_listeners.erase(listener);
	}
protected:
	std::set<MixerControlableChangeListener*> m_listeners;
};