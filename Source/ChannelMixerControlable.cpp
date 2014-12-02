#include "ChannelMixerControlable.h"

ChannelMixerControlable::ChannelMixerControlable(int channel, ChannelVolumeAudioSource* audioSource)
	: channelNumber(channel)
	, m_channelVolumeAudioSource(audioSource)
{
}

void ChannelMixerControlable::setGain(float gain)
{
	m_channelVolumeAudioSource->setChannelVolume(channelNumber, gain);
}

float ChannelMixerControlable::getGain() const
{
	return m_channelVolumeAudioSource->getChannelVolume(channelNumber);
}

void ChannelMixerControlable::setPan(float /*pan*/)
{
	jassertfalse
}

float ChannelMixerControlable::getPan() const
{
	jassertfalse
	return 0.0f;
}

void ChannelMixerControlable::setSoloMute(bool /*soloMute*/)
{
	jassertfalse;
}

bool ChannelMixerControlable::getSoloMute() const
{
	jassertfalse
	return false;
}

void ChannelMixerControlable::setSolo(bool solo)
{
	m_channelVolumeAudioSource->setChannelSolo(channelNumber, solo);
}

bool ChannelMixerControlable::getSolo() const
{
	return m_channelVolumeAudioSource->getChannelSolo(channelNumber);
}

void ChannelMixerControlable::setMute(bool mute)
{
	m_channelVolumeAudioSource->setChannelMute(channelNumber, mute);
}

bool ChannelMixerControlable::getMute() const
{
	return m_channelVolumeAudioSource->getChannelMute(channelNumber);
}

float ChannelMixerControlable::getVolume() const
{
	return m_channelVolumeAudioSource->getActualVolume(channelNumber);
}
