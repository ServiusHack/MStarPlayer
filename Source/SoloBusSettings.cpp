#include "SoloBusSettings.h"

void SoloBusSettings::setChannel(SoloBusChannel busChannel, int outputChannel)
{
    int oldValue = -1;
    switch (busChannel)
    {
    case SoloBusChannel::Left:
        oldValue = m_leftChannel;
        m_leftChannel = outputChannel;
        break;
    case SoloBusChannel::Right:
        oldValue = m_rightChannel;
        m_rightChannel = outputChannel;
        break;
    }

    for (SoloBusSettingsListener* listener : m_listeners)
        listener->soloBusChannelChanged(busChannel, outputChannel, oldValue);
}

int SoloBusSettings::getChannel(SoloBusChannel busChannel) const
{
    switch (busChannel)
    {
    case SoloBusChannel::Left:
        return m_leftChannel;
    case SoloBusChannel::Right:
        return m_rightChannel;
    }

    // Should never get here.
    jassertfalse;
    return -1;
}

bool SoloBusSettings::isConfigured() const
{
    return m_leftChannel != -1 || m_rightChannel != -1;
}

void SoloBusSettings::saveToXml(XmlElement* element) const
{
    XmlElement* leftChannelElement = new XmlElement("Left");
    leftChannelElement->addTextElement(String(m_leftChannel));
    element->addChildElement(leftChannelElement);

    XmlElement* rightChannelElement = new XmlElement("Right");
    rightChannelElement->addTextElement(String(m_rightChannel));
    element->addChildElement(rightChannelElement);
}

void SoloBusSettings::restoreFromXml(const XmlElement& element)
{
    XmlElement* leftChannelElement = element.getChildByName("Left");
    m_leftChannel = leftChannelElement->getAllSubText().trim().getIntValue();

    XmlElement* rightChannelElement = element.getChildByName("Right");
    m_rightChannel = rightChannelElement->getAllSubText().trim().getIntValue();

    for (SoloBusSettingsListener* listener : m_listeners)
    {
        listener->soloBusChannelChanged(SoloBusChannel::Left, m_leftChannel, -1);
        listener->soloBusChannelChanged(SoloBusChannel::Right, m_rightChannel, -1);
    }
}

void SoloBusSettings::addListener(SoloBusSettingsListener* listener)
{
    m_listeners.push_back(listener);
}

void SoloBusSettings::removeListener(SoloBusSettingsListener* listener)
{
    m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), listener));
}
