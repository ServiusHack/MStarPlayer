#include "OutputChannelNames.h"


OutputChannelNames::OutputChannelNames(AudioDeviceManager &deviceManager)
	: m_audioDevice(deviceManager.getCurrentAudioDevice())
{
	if (m_audioDevice) {
		m_deviceOutputChannelNames = m_audioDevice->getOutputChannelNames();
		m_internalOutputChannelNames = m_audioDevice->getOutputChannelNames();
	}
	deviceManager.addChangeListener(this);
}

void OutputChannelNames::changeListenerCallback(ChangeBroadcaster *source)
{
	AudioDeviceManager* manager = static_cast<AudioDeviceManager*>(source);

	if (m_audioDevice != manager->getCurrentAudioDevice())
	{
		m_audioDevice = manager->getCurrentAudioDevice();

		if (m_audioDevice) {
			m_deviceOutputChannelNames = m_audioDevice->getOutputChannelNames();
			m_internalOutputChannelNames = m_audioDevice->getOutputChannelNames();
		}


		for (OutputChannelNamesListener* listener : m_listeners)
			listener->outputChannelNamesReset();
	}

	sendChangeMessage();
}

int OutputChannelNames::getNumberOfChannels()
{
	return m_audioDevice->getActiveOutputChannels().countNumberOfSetBits();
}

String OutputChannelNames::getDeviceOutputChannelName(int activeChannelIndex)
{
	jassert(activeChannelIndex >= 0 && activeChannelIndex < m_deviceOutputChannelNames.size());

	BigInteger activeOutputChannels = m_audioDevice->getActiveOutputChannels();

	int outputChannel = -1;
	while (activeChannelIndex-- >= 0)
	{
		outputChannel = activeOutputChannels.findNextSetBit(outputChannel+1);
		jassert(outputChannel != -1);
	}

	return m_deviceOutputChannelNames[outputChannel];
}

String OutputChannelNames::getInternalOutputChannelName(int activeChannelIndex)
{
	jassert(activeChannelIndex >= 0 && activeChannelIndex < m_internalOutputChannelNames.size());

	BigInteger activeOutputChannels = m_audioDevice->getActiveOutputChannels();

	int outputChannel = -1;
	while (activeChannelIndex-- >= 0)
	{
		outputChannel = activeOutputChannels.findNextSetBit(outputChannel+1);
		jassert(outputChannel != -1);
	}
	return m_internalOutputChannelNames[outputChannel];
}
void OutputChannelNames::setInternalOutputChannelName(int activeChannelIndex, String text)
{
	int tmpActiveChannelIndex = activeChannelIndex;
	BigInteger activeOutputChannels = m_audioDevice->getActiveOutputChannels();

	int outputChannel = -1;
	while (tmpActiveChannelIndex-- >= 0)
	{
		outputChannel = activeOutputChannels.findNextSetBit(outputChannel+1);
		jassert(outputChannel != -1);
	}
	m_internalOutputChannelNames.set(outputChannel, text);

	for (OutputChannelNamesListener* listener : m_listeners)
		listener->outputChannelNameChanged(activeChannelIndex, text);
}

void OutputChannelNames::addListener(OutputChannelNamesListener* listener)
{
	m_listeners.push_back(listener);
}

void OutputChannelNames::removeListener(OutputChannelNamesListener* listener)
{
	m_listeners.remove(listener);
}