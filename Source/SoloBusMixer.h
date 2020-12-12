#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "ChannelMixerControlable.h"
#include "ChannelVolumeAudioSource.h"
#include "MixerFader.h"
#include "OutputChannelNames.h"
#include "PlayerMixerFader.h"
#include "SubchannelPlayer.h"

#include <memory>

class SoloBusMixer
    : public juce::Component
    , public SoloBusSettingsListener
    , public juce::Timer
{
public:
    SoloBusMixer(SoloBusSettings& soloBusSettings, ChannelVolumeAudioSource& channelVolumeAudioSource);
    ~SoloBusMixer();

    // XML Serialization
public:
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    void saveToXml(juce::XmlElement* element) const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml(const juce::XmlElement& element);

    // Slider for channels and players
private:
    ChannelMixerControlable m_leftMixer;
    ChannelMixerControlable m_rightMixer;
    MixerFader m_leftFader;
    MixerFader m_rightFader;
    SoloBusSettings& m_soloBusSettings;

    // Component
public:
    virtual void resized() override;

    // SoloBusSettingsListener
public:
    void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) override;

    // Timer
public:
    virtual void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBusMixer)
};
