#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "VolumeAnalyzer.h"

/**
	Audio source which remaps channels and tracks the current volume of each channel.
*/
class ChannelRemappingAudioSourceWithVolume
	: public AudioSource
{
public:
    ChannelRemappingAudioSourceWithVolume(AudioSource* source, bool deleteSourceWhenDeleted);

// Channel volume
public:
	float getVolume() const;

	double getSampleRate() const;

private:
	std::vector<VolumeAnalyzer> m_volumes;
	size_t m_bufferSize;
	double m_sampleRate;

// ChannelRemappingAudioSource
public:
    //==============================================================================
    /** Specifies a number of channels that this audio source must produce from its
        getNextAudioBlock() callback.
    */
	void setNumberOfChannelsToProduce(int requiredNumberOfChannels);

    /** Clears any mapped channels.

        After this, no channels are mapped, so this object will produce silence. Create
        some mappings with setInputChannelMapping() and setOutputChannelMapping().
    */
    void clearAllMappings();

    /** Creates an output channel mapping.

        When the getNextAudioBlock() method is called, the data returned in channel sourceChannelIndex by
        our input audio source will be copied to channel destChannelIndex of the final buffer.

        @param sourceChannelIndex   the index of an output channel coming from our input audio source
                                    (i.e. the source specified when this object was created).
        @param destChannelIndex     the index of the output channel in the incoming audio data buffer
                                    during our getNextAudioBlock() callback
    */
    void setOutputChannelMapping (int sourceChannelIndex,
                                  int destChannelIndex);

	void setOutputChannelMappingInternal(const int sourceIndex, const int destIndex);

    /** Returns the output channel to which channel outputChannelIndex of our input audio
        source will be sent to.
    */
    int getRemappedOutputChannel (int outputChannelIndex) const;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
	virtual void getNextAudioBlock(const AudioSourceChannelInfo&) override;

    //==============================================================================
    /** Returns an XML object to encapsulate the state of the mappings.
        @see restoreFromXml
    */
    XmlElement* createXml() const;

    /** Restores the mappings from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml (const XmlElement&);

private:
    OptionalScopedPointer<AudioSource> source;
    Array<std::pair<int,int>> remappedOutputs;
    int requiredNumberOfChannels;

    AudioSampleBuffer buffer;
    AudioSourceChannelInfo remappedInfo;
    CriticalSection lock;
};