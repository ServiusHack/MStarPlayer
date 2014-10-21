/*
  ==============================================================================

    ChannelVolumeAudioSource.cpp
    Created: 25 Oct 2013 10:31:48pm
    Author:  Severin Leonhardt

  ==============================================================================
*/

#include "ChannelVolumeAudioSource.h"


ChannelVolumeAudioSource::ChannelVolumeAudioSource (AudioSource* const source_)
   : source (source_)
{
}

void ChannelVolumeAudioSource::resetAllVolumes()
{
    const ScopedLock sl (lock);

    volumes.clear();
}

void ChannelVolumeAudioSource::setChannelVolume (const int channelIndex, float gain)
{
    const ScopedLock sl (lock);

    while (volumes.size() < channelIndex)
        volumes.add (1.0);

    volumes.set (channelIndex, gain);
}

float ChannelVolumeAudioSource::getChannelVolume (const int channelIndex) const
{
    const ScopedLock sl (lock);

    if (channelIndex >= 0 && channelIndex < volumes.size())
        return volumes.getUnchecked (channelIndex);

    return -1.0;
}

//==============================================================================
void ChannelVolumeAudioSource::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    source->prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void ChannelVolumeAudioSource::releaseResources()
{
    source->releaseResources();
}

void ChannelVolumeAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    const ScopedLock sl (lock);

    source->getNextAudioBlock (bufferToFill);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++)
    {
        float gain = 1.0;
        if (channel < volumes.size())
            gain = volumes.getUnchecked(channel);
        bufferToFill.buffer->applyGain(channel, bufferToFill.startSample, bufferToFill.numSamples, gain);
    }
}