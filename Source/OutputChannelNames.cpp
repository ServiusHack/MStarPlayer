#include "OutputChannelNames.h"


bool DeviceIdentification::operator!=(const DeviceIdentification& other)
{
	return deviceTypeName != other.deviceTypeName || deviceName != other.deviceName;
}

OutputChannelNames::OutputChannelNames(AudioDeviceManager &deviceManager)
	: m_audioDevice(deviceManager.getCurrentAudioDevice())
{
	if (m_audioDevice) {
		m_deviceOutputChannelNames = m_audioDevice->getOutputChannelNames();
		m_internalOutputChannelNames = m_audioDevice->getOutputChannelNames();
		m_deviceIdentification = ExtractDeviceIdentification(m_audioDevice);
	}
	deviceManager.addChangeListener(this);
}

DeviceIdentification OutputChannelNames::ExtractDeviceIdentification(AudioIODevice* audioDevice)
{
	if (audioDevice)
		return DeviceIdentification{ audioDevice->getTypeName(), audioDevice->getName() };
	else
		return DeviceIdentification();
}

void OutputChannelNames::changeListenerCallback(ChangeBroadcaster *source)
{
	AudioDeviceManager* manager = static_cast<AudioDeviceManager*>(source);

	if (m_deviceIdentification != ExtractDeviceIdentification(manager->getCurrentAudioDevice()))
	{
		m_audioDevice = manager->getCurrentAudioDevice();

		if (m_audioDevice) {
			m_deviceOutputChannelNames = m_audioDevice->getOutputChannelNames();
			m_internalOutputChannelNames = m_audioDevice->getOutputChannelNames();
			m_deviceIdentification = ExtractDeviceIdentification(m_audioDevice);
		}
		else {
			m_deviceOutputChannelNames.clear();
			m_internalOutputChannelNames.clear();
			m_deviceIdentification = DeviceIdentification();
		}

		for (OutputChannelNamesListener* listener : m_listeners)
			listener->outputChannelNamesReset();
	}

	sendChangeMessage();
}

int OutputChannelNames::getNumberOfChannels()
{
	if (m_audioDevice)
		return m_audioDevice->getActiveOutputChannels().countNumberOfSetBits();
	else
		return 0;
}

StringArray OutputChannelNames::getAllDeviceOutputChannelNames()
{
	StringArray names;
	for (int i = 0; i < getNumberOfChannels(); ++i)
		names.add(getInternalOutputChannelName(i));
	return names;
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
void OutputChannelNames::setInternalOutputChannelName(int activeChannelIndex, const String& text)
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
	m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), listener));
}

void OutputChannelNames::saveToXml(XmlElement* element) const
{
	for (int i = 0; i < m_internalOutputChannelNames.size(); ++i)
	{
		XmlElement* channelElement = new XmlElement("Name");
		channelElement->addTextElement(m_internalOutputChannelNames[i]);
		element->addChildElement(channelElement);
	}
}

void OutputChannelNames::restoreFromXml(const XmlElement &element)
{
	for (int i = 0; i < element.getNumChildElements(); ++i)
	{
		String channelName = element.getChildElement(i)->getAllSubText();
		m_internalOutputChannelNames.set(i, channelName);
	}

	for (OutputChannelNamesListener* listener : m_listeners)
		listener->outputChannelNamesReset();
}