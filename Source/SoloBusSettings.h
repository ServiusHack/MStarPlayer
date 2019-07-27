#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

enum class SoloBusChannel
{
    Left,
    Right
};

/**
        Interface to receive changes to the output channel names.
*/
class SoloBusSettingsListener
{
public:
    virtual void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) = 0;
};

/**
        Maintains the user selected solo bus channels.
*/
class SoloBusSettings : public ChangeBroadcaster
{
public:
    void setChannel(SoloBusChannel busChannel, int outputChannel);
    int getChannel(SoloBusChannel busChannel) const;

    bool isConfigured() const;

private:
    int m_leftChannel{-1};
    int m_rightChannel{-1};

    // XML serialization
public:
    void saveToXml(XmlElement* element) const;
    void restoreFromXml(const XmlElement& element);

    // SoloBusSettingsListener management
public:
    /**
                Add a listener which will be notified when a bus channel is assigned an output channel.

                The ownership will not be transfered, it is the caller's responsibility to remove and destroy the
       listener later.
        */
    void addListener(SoloBusSettingsListener* listener);

    void removeListener(SoloBusSettingsListener* listener);

private:
    std::vector<SoloBusSettingsListener*> m_listeners;
};
