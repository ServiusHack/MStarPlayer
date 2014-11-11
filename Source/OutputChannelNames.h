#pragma once

#include <list>

#include "../JuceLibraryCode/JuceHeader.h"

class OutputChannelNamesListener
{
public:
	virtual void outputChannelNamesReset() {};
	virtual void outputChannelNameChanged(int /*activeChannelIndex*/, String /*text*/) {};
};

class OutputChannelNames
	: public ChangeListener
	, public ChangeBroadcaster
{
public:
	OutputChannelNames(AudioDeviceManager &deviceManager);

	void addListener();

	int getNumberOfChannels();

	String getDeviceOutputChannelName(int activeChannelIndex);

	String getInternalOutputChannelName(int activeChannelIndex);

	void setInternalOutputChannelName(int activeChannelIndex, String text);

	virtual void changeListenerCallback(ChangeBroadcaster *source) override;

	void addListener(OutputChannelNamesListener* listener);

	void removeListener(OutputChannelNamesListener* listener);

private:
	AudioIODevice* m_audioDevice;
	StringArray m_deviceOutputChannelNames;
	StringArray m_internalOutputChannelNames;

	std::list<OutputChannelNamesListener*> m_listeners;
};