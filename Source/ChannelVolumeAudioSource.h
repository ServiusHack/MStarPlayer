#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "boost/circular_buffer.hpp"

/**
	Audio source which applies a gain, solo and mute state to individual channels.
*/
class ChannelVolumeAudioSource : public AudioSource
{
public:
    /** Creates a volume changing source that will pass on audio from the given input.

        @param source       the input source to use. Make sure that this doesn't
                            get deleted before the ChannelVolumeAudioSource object
        @param deleteSourceWhenDeleted  if true, the input source will be deleted
                            when this object is deleted, if false, the caller is
                            responsible for its deletion
    */
    ChannelVolumeAudioSource(AudioSource* source);

	int channelCount() const;

private:
	void expandListsTo(size_t channelIndex);
	AudioSource* m_source;
    CriticalSection m_lock;
    
// volume
public:
    /** Resets all volumes.

        After this all channels will have their default volume.
        This means there will be no change in volume for any channel.
    */
    void resetAllVolumes();

    /** Sets the volume of a channel.

        When the getNextAudioBlock() method is called, the data in channel channelIndex will be adjusted by the gain.

        @param channelIndex         the index of an channel in our input audio source (i.e. the
                                    source specified when this object was created).
        @param gain                 the gain to multiply the samples with during our getNextAudioBlock() callback
    */
    void setChannelVolume(size_t channelIndex, float gain);

    /** Returns the volume from a channel.
    */
	float getChannelVolume(size_t channelIndex) const;

private:
    std::vector<float> m_setVolumes;

// solo
public:
	void setChannelSolo(size_t channelIndex, bool solo);
	bool getChannelSolo(size_t channelIndex) const;

private:
	std::vector<bool> m_setSolos;
	bool m_anySolo;

	// mute
public:
	void setChannelMute(size_t channelIndex, bool mute);
	bool getChannelMute(size_t channelIndex) const;

private:
	std::vector<bool> m_setMutes;

// actual output volume
public:
	float getActualVolume(size_t channelIndex) const;

private:
	std::vector<boost::circular_buffer<float>> m_actualVolumes;
	size_t m_bufferSize;

// gain to apply
private:
	void updateGain(size_t channelIndex);
	std::vector<float> m_appliedGains;

// AudioSource
public:
    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	virtual void releaseResources() override;
	virtual void getNextAudioBlock(const AudioSourceChannelInfo&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelVolumeAudioSource)
};
