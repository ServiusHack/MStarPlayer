#pragma once

#include "ChannelVolumeAudioSource.h"
#include "Player.h"

/**
        MixerControlable for an individual channel.
*/
class ChannelMixerControlable : public MixerControlable
{
public:
    ChannelMixerControlable(
        int channel, ChannelVolumeAudioSource* audioSource, const juce::String& name = juce::String());

    void setChannel(int channel);

    virtual void setGain(float gain) override;

    virtual float getGain() const override;

    virtual void setPan(float pan) override;

    virtual float getPan() const override;

    virtual void setSoloMute(bool soloMute) override;

    virtual bool getSoloMute() const override;

    virtual void setSolo(bool solo) override;

    virtual bool getSolo() const override;

    virtual void setMute(bool mute) override;

    virtual bool getMute() const override;

    virtual float getVolume() const override;

    virtual juce::String getName() const override;

private:
    int channelNumber;
    ChannelVolumeAudioSource* m_channelVolumeAudioSource;
    juce::String m_name;
};
