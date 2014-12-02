#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

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

	void setChannelSolo(size_t channelIndex, bool solo);

	bool getChannelSolo(size_t channelIndex) const;

	void setChannelMute(size_t channelIndex, bool mute);

	bool getChannelMute(size_t channelIndex) const;

	void updateGain(size_t channelIndex);

	float getActualVolume(size_t channelIndex) const;

	int channelCount() const;

    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	virtual void releaseResources() override;
	virtual void getNextAudioBlock(const AudioSourceChannelInfo&) override;

private:
	void expandListsTo(size_t channelIndex);

	AudioSource* m_source;
	std::vector<float> m_appliedGains;
    std::vector<float> m_setVolumes;
	std::vector<bool> m_setMutes;
	std::vector<bool> m_setSolos;
	std::vector<float> m_actualVolumes;

	bool m_anySolo;

    CriticalSection m_lock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelVolumeAudioSource)
};
