#pragma once

#include <list>

#include "../JuceLibraryCode/JuceHeader.h"

class OutputChannelNamesListener
{
public:
	virtual void outputChannelNamesReset() = 0;
	virtual void outputChannelNameChanged(int activeChannelIndex, String text) = 0;
};

/**
	Maintains the user defined output channel names.
*/
class OutputChannelNames
	: public ChangeListener
	, public ChangeBroadcaster
{
public:
	OutputChannelNames(AudioDeviceManager &deviceManager);

	int getNumberOfChannels();

	String getDeviceOutputChannelName(int activeChannelIndex);

	String getInternalOutputChannelName(int activeChannelIndex);

	void setInternalOutputChannelName(int activeChannelIndex, String text);

	virtual void changeListenerCallback(ChangeBroadcaster *source) override;

	/**
		Add a listener which will be notified when a channel name changes or all names are reset.

		The ownership will not be transfered, it is the caller's responsibility to remove and destroy the listener later.
	*/
	void addListener(OutputChannelNamesListener* listener);

	void removeListener(OutputChannelNamesListener* listener);

	XmlElement* saveToXml();
	void restoreFromXml(XmlElement &element);

private:
	AudioIODevice* m_audioDevice;
	StringArray m_deviceOutputChannelNames;
	StringArray m_internalOutputChannelNames;

	std::list<OutputChannelNamesListener*> m_listeners;
};