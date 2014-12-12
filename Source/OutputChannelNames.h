#pragma once

#include <list>

#include "../JuceLibraryCode/JuceHeader.h"

class OutputChannelNamesListener
{
public:
	virtual void outputChannelNamesReset() = 0;
	virtual void outputChannelNameChanged(int activeChannelIndex, const String& text) = 0;
};

/**
	Maintains the user defined output channel names.
*/
class OutputChannelNames
	: public ChangeListener
	, public ChangeBroadcaster
{
public:
	OutputChannelNames(AudioDeviceManager& deviceManager);

	int getNumberOfChannels();

private:
	AudioIODevice* m_audioDevice;

// output device names and their internal names
public:
	String getDeviceOutputChannelName(int activeChannelIndex);
	StringArray getAllDeviceOutputChannelNames();
	String getInternalOutputChannelName(int activeChannelIndex);
	void setInternalOutputChannelName(int activeChannelIndex, const String& text);

private:
	StringArray m_deviceOutputChannelNames;
	StringArray m_internalOutputChannelNames;

// XML serialization
public:
	XmlElement* saveToXml();
	void restoreFromXml(const XmlElement &element);

// OutputChannelNamesListener management
public:
	/**
		Add a listener which will be notified when a channel name changes or all names are reset.

		The ownership will not be transfered, it is the caller's responsibility to remove and destroy the listener later.
	*/
	void addListener(OutputChannelNamesListener* listener);

	void removeListener(OutputChannelNamesListener* listener);

private:
	std::list<OutputChannelNamesListener*> m_listeners;

// ChangeListener
public:
	virtual void changeListenerCallback(ChangeBroadcaster* source) override;
};