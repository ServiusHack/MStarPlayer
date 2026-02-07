#include "OutputChannelNames.h"

bool DeviceIdentification::operator!=(const DeviceIdentification& other)
{
    return deviceTypeName != other.deviceTypeName || deviceName != other.deviceName;
}

OutputChannelNames::OutputChannelNames(juce::AudioDeviceManager& deviceManager)
    : m_audioDevice(deviceManager.getCurrentAudioDevice())
{
    if (m_audioDevice)
    {
        m_deviceOutputChannelNames = m_audioDevice->getOutputChannelNames();
        m_internalOutputChannelNames = m_audioDevice->getOutputChannelNames();
        m_pairingModes.resize(m_audioDevice->getOutputChannelNames().size());
        m_pairingModes.fill(PairingMode::Mono);
        m_deviceIdentification = ExtractDeviceIdentification(m_audioDevice);
    }
    deviceManager.addChangeListener(this);
}

DeviceIdentification OutputChannelNames::ExtractDeviceIdentification(const juce::AudioIODevice* audioDevice)
{
    if (audioDevice)
        return DeviceIdentification{audioDevice->getTypeName(), audioDevice->getName()};
    else
        return DeviceIdentification();
}

void OutputChannelNames::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    const juce::AudioDeviceManager* manager = static_cast<juce::AudioDeviceManager*>(source);

    if (m_deviceIdentification != ExtractDeviceIdentification(manager->getCurrentAudioDevice()))
    {
        m_audioDevice = manager->getCurrentAudioDevice();

        if (m_audioDevice)
        {
            m_deviceOutputChannelNames = m_audioDevice->getOutputChannelNames();
            m_internalOutputChannelNames = m_audioDevice->getOutputChannelNames();
            m_pairingModes.resize(m_audioDevice->getOutputChannelNames().size());
            m_pairingModes.fill(PairingMode::Mono);
            m_deviceIdentification = ExtractDeviceIdentification(m_audioDevice);
        }
        else
        {
            m_deviceOutputChannelNames.clear();
            m_internalOutputChannelNames.clear();
            m_pairingModes.clear();
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

bool OutputChannelNames::SetChannelPairing(int activeChannelIndex, PairingMode mode)
{
    jassert(activeChannelIndex >= 0 && activeChannelIndex < m_pairingModes.size());

    const PairingMode previousMode = m_pairingModes[activeChannelIndex];
    m_pairingModes.set(activeChannelIndex, mode);

    for (OutputChannelNamesListener* listener : m_listeners)
        listener->outputChannelPairingModeChanged(activeChannelIndex, mode);

    switch (mode)
    {
    case PairingMode::Mono:
        if (previousMode == PairingMode::Left && activeChannelIndex + 1 < m_pairingModes.size())
        {
            m_pairingModes.set(activeChannelIndex + 1, PairingMode::Mono);

            for (OutputChannelNamesListener* listener : m_listeners)
                listener->outputChannelPairingModeChanged(activeChannelIndex + 1, PairingMode::Mono);
        }
        else if (previousMode == PairingMode::Right && activeChannelIndex - 1 >= 0)
        {
            m_pairingModes.set(activeChannelIndex - 1, PairingMode::Mono);

            for (OutputChannelNamesListener* listener : m_listeners)
                listener->outputChannelPairingModeChanged(activeChannelIndex - 1, PairingMode::Mono);
        }
        break;
    case PairingMode::Left:
        if (activeChannelIndex + 1 < m_pairingModes.size())
        {
            m_pairingModes.set(activeChannelIndex + 1, PairingMode::Right);

            for (OutputChannelNamesListener* listener : m_listeners)
                listener->outputChannelPairingModeChanged(activeChannelIndex + 1, PairingMode::Right);
        }
        else
        {
        }
        break;
    case PairingMode::Right:
        if (activeChannelIndex - 1 >= 0)
        {
            m_pairingModes.set(activeChannelIndex - 1, PairingMode::Left);

            for (OutputChannelNamesListener* listener : m_listeners)
                listener->outputChannelPairingModeChanged(activeChannelIndex - 1, PairingMode::Left);
        }
        else
        {
        }
        break;
    }

    return previousMode != mode;
}

PairingMode OutputChannelNames::GetChannelPairing(int activeChannelIndex)
{
    jassert(activeChannelIndex >= 0 && activeChannelIndex < m_pairingModes.size());

    return m_pairingModes[activeChannelIndex];
}

juce::StringArray OutputChannelNames::getAllDeviceOutputChannelNames()
{
    juce::StringArray names;
    for (int i = 0; i < getNumberOfChannels(); ++i)
        names.add(getInternalOutputChannelName(i));
    return names;
}

juce::String OutputChannelNames::getDeviceOutputChannelName(int activeChannelIndex)
{
    jassert(activeChannelIndex >= 0 && activeChannelIndex < m_deviceOutputChannelNames.size());

    juce::BigInteger activeOutputChannels = m_audioDevice->getActiveOutputChannels();

    int outputChannel = -1;
    while (activeChannelIndex-- >= 0)
    {
        outputChannel = activeOutputChannels.findNextSetBit(outputChannel + 1);
        jassert(outputChannel != -1);
    }

    return m_deviceOutputChannelNames[outputChannel];
}

juce::String OutputChannelNames::getInternalOutputChannelName(int activeChannelIndex)
{
    jassert(activeChannelIndex >= 0 && activeChannelIndex < m_internalOutputChannelNames.size());

    juce::BigInteger activeOutputChannels = m_audioDevice->getActiveOutputChannels();

    int outputChannel = -1;
    while (activeChannelIndex-- >= 0)
    {
        outputChannel = activeOutputChannels.findNextSetBit(outputChannel + 1);
        jassert(outputChannel != -1);
    }
    return m_internalOutputChannelNames[outputChannel];
}

void OutputChannelNames::setInternalOutputChannelName(int activeChannelIndex, const juce::String& text)
{
    int tmpActiveChannelIndex = activeChannelIndex;
    juce::BigInteger activeOutputChannels = m_audioDevice->getActiveOutputChannels();

    int outputChannel = -1;
    while (tmpActiveChannelIndex-- >= 0)
    {
        outputChannel = activeOutputChannels.findNextSetBit(outputChannel + 1);
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

void OutputChannelNames::saveToXml(juce::XmlElement* element) const
{
    for (int i = 0; i < m_internalOutputChannelNames.size(); ++i)
    {
        juce::XmlElement* channelElement = new juce::XmlElement("Name");
        channelElement->addTextElement(m_internalOutputChannelNames[i]);
        element->addChildElement(channelElement);
    }
}

void OutputChannelNames::restoreFromXml(const juce::XmlElement& element)
{
    for (int i = 0; i < element.getNumChildElements(); ++i)
    {
        juce::String channelName = element.getChildElement(i)->getAllSubText();
        m_internalOutputChannelNames.set(i, channelName);
    }

    for (OutputChannelNamesListener* listener : m_listeners)
        listener->outputChannelNamesReset();
}
