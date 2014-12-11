/*
  ==============================================================================

	Track.cpp
	Created: 21 Jan 2014 1:00:48am
	Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Track.h"

#include <sstream>

Track::Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback, float gain, bool mute, ChannelCountChangedCallback channelCountChangedCallback, PlayingStateChangedCallback playingStateChangedCallback)
	: m_trackIndex(trackIndex)
	, m_stereo(stereo)
	, m_tracksMixer(tracksMixer)
	, m_thread("track")
	, m_durationChangedCallback(callback)
	, m_soloMute(soloMute)
	, m_soloChangedCallback(soloChangedCallback)
	, m_playerGain(gain)
	, m_trackGain(1.0f)
	, m_playerMute(mute)
	, m_channelCountChangedCallback(channelCountChangedCallback)
	, m_playingStateChangedCallback(playingStateChangedCallback)
	, m_mute(false)
	, m_solo(false)
	, m_audioThumbnailCache(1)
	, m_outputChannels(outputChannels)
	, m_remappingAudioSource(&m_transportSource, false)
	, m_audioThumbnail(1000, m_formatManager, m_audioThumbnailCache)
{
	m_formatManager.registerBasicFormats();
	m_thread.startThread(3);

	m_transportSource.setGain(gain);

	m_remappingAudioSource.setNumberOfChannelsToProduce(outputChannels);
	m_remappingAudioSource.setOutputChannelMapping(0, 0);
	m_remappingAudioSource.setOutputChannelMapping(1, 1);

	m_tracksMixer.addInputSource(&m_remappingAudioSource, false);
}

Track::~Track()
{
	m_tracksMixer.removeInputSource(&m_remappingAudioSource);
}

void Track::setName(String name)
{
	m_name = name;
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->nameChanged(name);
}

String Track::getName() const
{
	return m_name;
}

void Track::setPlayerGain(float gain)
{
	m_playerGain = gain;
	updateGain();
}

void Track::setGain(float gain)
{
	m_trackGain = gain;
	updateGain();
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->gainChanged(gain);
}

void Track::setPlayerMute(bool mute)
{
	m_playerMute = mute;
	updateGain();
}

void Track::setMute(bool mute)
{
	m_mute = mute;
	updateGain();
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->muteChanged(mute);
}

float Track::getGain() const
{
	return m_trackGain;
}

bool Track::getMute() const
{
	return m_mute;
}

void Track::setSolo(bool solo)
{
	m_solo = solo;
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->soloChanged(solo);
	m_soloChangedCallback();
}

float Track::getVolume() const
{
	float maxVolume = 0;
	for (int i = 0; i < m_outputChannels; ++i)
		maxVolume = std::max(maxVolume, m_remappingAudioSource.getVolume(i));
	return maxVolume;
}

void Track::play()
{
	if (m_transportSource.hasStreamFinished())
		setPosition(0);
	m_transportSource.start();
	startTimer(50);
	m_playingStateChangedCallback(true);
}

void Track::pause()
{
	if (m_transportSource.isPlaying()) {
		m_transportSource.stop();
		stopTimer();
		m_playingStateChangedCallback(false);
	}
	else {
		m_transportSource.start();
		startTimer(50);
		m_playingStateChangedCallback(true);
	}
}

void Track::stop()
{
	m_transportSource.stop();
	setPosition(0);
	stopTimer();
	m_playingStateChangedCallback(false);
}

void Track::setPosition(double position)
{
	m_transportSource.setPosition(position);
	callPositionCallbacks(position);
}

std::vector<int> Track::getMapping()
{
	int numChannels = m_stereo ? 2 : 1;
	std::vector<int> mapping(numChannels, -1);
	for (size_t channel = 0; channel < mapping.size(); ++channel)
		mapping[channel] = (m_remappingAudioSource.getRemappedOutputChannel(channel));

	return mapping;
}

void Track::setOutputChannels(int outputChannels)
{
	m_outputChannels = outputChannels;
	m_remappingAudioSource.setNumberOfChannelsToProduce(outputChannels);
}

void Track::setOutputChannelMapping(int source, int target)
{
	m_remappingAudioSource.setOutputChannelMapping(source, target);
}

int Track::getNumChannels()
{
	return m_currentAudioFileSource->getAudioFormatReader()->numChannels;
}

double Track::getDuration()
{
	return m_duration;
}

void Track::setSoloMute(bool mute)
{
	m_soloMute = mute;
	updateGain();
}

bool Track::getSoloMute() const
{
	return m_soloMute;
}

Track::PositionCallbackRegistrationToken Track::addPositionCallback(PositionCallback callback)
{
	this->m_positionCallbacks.push_front(callback);
	return this->m_positionCallbacks.cbegin();
}

void Track::unregisterPositionCallback(PositionCallbackRegistrationToken& token)
{
	m_positionCallbacks.erase(token);
}

void Track::setFileChangedCallback(FileChangedCallback fileChangedCallback)
{
	m_fileChangedCallback = fileChangedCallback;
}

void Track::loadTrackConfig(const TrackConfig& config)
{
	loadFileIntoTransport(File(config.file));
}

TrackConfig Track::getTrackConfig()
{
	TrackConfig config;
	config.file = m_audioFile.getFullPathName();
	return config;
}

bool Track::isPlaying()
{
	return m_transportSource.isPlaying();
}

void Track::updateGain()
{
	bool mute = m_playerMute || m_mute || (m_soloMute && !m_solo);
	m_transportSource.setGain(mute ? 0.0f : (m_playerGain * m_trackGain));
}

bool Track::getSolo() const
{
	return m_solo;
}


void Track::loadFileIntoTransport(File audioFile)
{
	m_audioFile = audioFile;

	AudioFormatReader* reader = m_formatManager.createReaderFor(m_audioFile);

	m_transportSource.setSource(nullptr);

	m_audioThumbnail.setSource(new FileInputSource(m_audioFile));

	if (reader != nullptr) {
		m_currentAudioFileSource = new AudioFormatReaderSource(reader, true);
		// ..and plug it into our transport source
		m_transportSource.setSource(m_currentAudioFileSource,
									32768, // tells it to buffer this many samples ahead
									&m_thread, // this is the background thread to use for reading-ahead
									reader->sampleRate);
	}
	else {
		m_transportSource.setSource(nullptr);
		m_currentAudioFileSource = nullptr;
	}

	updateGain();

	m_duration = m_transportSource.getLengthInSeconds();
	m_durationChangedCallback();

	m_fileChangedCallback(m_audioFile.getFileName());
}

void Track::reloadFile()
{
	loadFileIntoTransport(m_audioFile);
}

void Track::unloadFile()
{
	loadFileIntoTransport(File::nonexistent);
}

void Track::timerCallback()
{
	callPositionCallbacks(m_transportSource.getCurrentPosition());

	if (!m_transportSource.isPlaying()) {
		stopTimer();
		m_playingStateChangedCallback(false);
	}
}

void Track::callPositionCallbacks(double position)
{
	// Copy list so the callback can remove items and we continue to iterate.
	std::list<PositionCallback> positionCallbacks(m_positionCallbacks);

	for (const auto& callback : positionCallbacks)
		callback(position, m_transportSource.hasStreamFinished());
}

XmlElement* Track::saveToXml() const
{
	XmlElement* element = new XmlElement("Track");

	element->setAttribute("stereo", m_stereo ? "true" : "false");
	element->setAttribute("mute", m_mute ? "true" : "false");
	element->setAttribute("solo", m_solo ? "true" : "false");
	element->setAttribute("gain", m_trackGain);


	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(getName());
	element->addChildElement(nameXml);

	return element;
}

void Track::restoreFromXml(const XmlElement& element)
{
	m_stereo = element.getStringAttribute("stereo", "false") == "true";
	setMute(element.getStringAttribute("mute", "false") == "true");
	setSolo(element.getStringAttribute("solo", "false") == "true");
	setGain(static_cast<float>(element.getDoubleAttribute("gain", 1.0)));

	XmlElement* nameXml = element.getChildByName("Name");
	if (nameXml != nullptr)
		setName(nameXml->getAllSubText().trim());
}

AudioFormatManager& Track::getAudioFormatManager()
{
	return m_formatManager;
}

bool Track::isStereo() const
{
	return m_stereo;
}

void Track::setStereo(bool stereo)
{
	m_stereo = stereo;
	m_channelCountChangedCallback();
}

int Track::getTrackIndex() const
{
	return m_trackIndex;
}

void Track::setTrackIndex(int index)
{
	m_trackIndex = index;
}

AudioThumbnail& Track::getAudioThumbnail()
{
	return m_audioThumbnail;
}
