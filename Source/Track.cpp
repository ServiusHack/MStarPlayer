#include "Track.h"

#include <cassert>
#include <sstream>

Track::Track(juce::MixerAudioSource& tracksMixer, SoloBusSettings& soloBusSettings, int trackIndex, bool stereo,
    int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback,
    float gain, bool mute, ChannelCountChangedCallback channelCountChangedCallback,
    PlayingStateChangedCallback playingStateChangedCallback, TrackConfigChangedCallback trackConfigChangedCallback,
    GainChangedCallback gainChangedCallback, juce::AudioThumbnailCache& audioThumbnailCache,
    juce::TimeSliceThread& thread)
    : m_trackIndex(trackIndex)
    , m_stereo(stereo)
    , m_tracksMixer(tracksMixer)
    , m_thread(thread)
    , m_durationChangedCallback(callback)
    , m_soloMute(soloMute)
    , m_soloChangedCallback(soloChangedCallback)
    , m_playerGain(gain)
    , m_trackGain(1.0f)
    , m_playerMute(mute)
    , m_channelCountChangedCallback(channelCountChangedCallback)
    , m_playingStateChangedCallback(playingStateChangedCallback)
    , m_trackConfigChangedCallback(trackConfigChangedCallback)
    , m_gainChangedCallback(gainChangedCallback)
    , m_mute(false)
    , m_solo(false)
    , m_pan(stereo ? 0.0f : NAN)
    , m_playerSolo(false)
    , m_audioThumbnailCache(audioThumbnailCache)
    , m_soloBusSettings(soloBusSettings)
    , m_remappingAudioSource(&m_transportSource, soloBusSettings, false)
    , m_audioThumbnail(1000, m_formatManager, m_audioThumbnailCache)
    , m_loadingTrackConfig(false)
{
    m_formatManager.registerBasicFormats();

    m_transportSource.setGain(gain);

    m_remappingAudioSource.setNumberOfChannelsToProduce(outputChannels);
    m_remappingAudioSource.setOutputChannelMapping(0, 0);
    m_remappingAudioSource.setOutputChannelMapping(1, 1);

    m_tracksMixer.addInputSource(&m_remappingAudioSource, false);
    m_soloBusSettings.addListener(this);
}

Track::~Track()
{
    m_tracksMixer.removeInputSource(&m_remappingAudioSource);
    m_soloBusSettings.removeListener(this);
}

void Track::setName(juce::String name)
{
    m_name = name;
    for (MixerControlableChangeListener* listener : m_listeners)
        listener->nameChanged(name);
}

juce::String Track::getName() const
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
    for (MixerControlableChangeListener* listener : m_listeners)
        listener->gainChanged(gain);

    m_gainChangedCallback(getName().toRawUTF8(), m_trackGain);
}

void Track::setPlayerSolo(bool solo)
{
    m_playerSolo = solo;
    if (m_soloBusSettings.isConfigured())
        m_remappingAudioSource.setSolo(m_solo || m_playerSolo);
    else
        updateGain();
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
    for (MixerControlableChangeListener* listener : m_listeners)
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
    for (MixerControlableChangeListener* listener : m_listeners)
        listener->soloChanged(solo);
    if (m_soloBusSettings.isConfigured())
        m_remappingAudioSource.setSolo(m_solo || m_playerSolo);
    else
        m_soloChangedCallback();
}

float Track::getVolume() const
{
    return m_remappingAudioSource.getVolume();
}

void Track::play()
{
    if (m_currentAudioFileSource)
    {
        setPosition(0);
        m_transportSource.start();
        startTimer(50);
        m_playingStateChangedCallback(true);
    }
    else
    {
        m_playingStateChangedCallback(false);
    }
}

void Track::pause()
{
    if (m_transportSource.isPlaying())
    {
        m_transportSource.stop();
        stopTimer();
        m_playingStateChangedCallback(false);
    }
    else if (m_currentAudioFileSource)
    {
        m_transportSource.start();
        startTimer(50);
        m_playingStateChangedCallback(true);
    }
    else
    {
        m_playingStateChangedCallback(false);
    }
}

void Track::stop()
{
    if (m_currentAudioFileSource)
    {
        m_transportSource.stop();
        setPosition(0);
    }
    stopTimer();
    m_playingStateChangedCallback(false);
}

void Track::setPosition(double position)
{
    m_transportSource.setPosition(position);
    callPositionCallbacks(position, m_transportSource.hasStreamFinished());
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
    m_remappingAudioSource.setNumberOfChannelsToProduce(outputChannels);
}

void Track::setOutputChannelMapping(int source, int target)
{
    m_remappingAudioSource.setOutputChannelMapping(source, target);
}

int Track::getNumChannels()
{
    return m_stereo ? 2 : 1;
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

void Track::setPan(const float pan)
{
    m_pan = pan;
    assert(-1.0f <= m_pan && m_pan <= 1.0f);
    // Mapping of the value range:
    // m_pan =  -1.0 ... 0.3 ... 0.0 ... 0.3 ... 1.0
    // clmp1 =   0.0 ... 0.0 ... 0.0 ... 0.3 ... 1.0 # clamp(0.0, 1.0)
    // left  =   1.0 ... 1.0 ... 1.0 ... 0.7 ... 0.0 # 1.0 - clmp1
    // clmp2 =  -1.0 ... 0.3 ... 0.0 ... 0.0 ... 0.0 # clamp(-1.0, 0.0)
    // right =   0.0 ... 0.7 ... 1.0 ... 1.0 ... 1.0 # 1.0 - clmp2
    const float gainLeft = 1.0 - std::clamp(m_pan, 0.0f, 1.0f);
    const float gainRight = 1.0 + std::clamp(m_pan, -1.0f, 0.0f);
    m_remappingAudioSource.setSourceChannelGain(0, gainLeft);
    m_remappingAudioSource.setSourceChannelGain(1, gainRight);
}

float Track::getPan() const
{
    return m_pan;
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
    m_loadingTrackConfig = true;
    loadFileIntoTransport(juce::File(config.file));
    m_loadingTrackConfig = false;
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
    bool mute = m_playerMute || m_mute || (!m_soloBusSettings.isConfigured() && m_soloMute && !m_solo);
    m_transportSource.setGain(mute ? 0.0f : (m_playerGain * m_trackGain));
}

bool Track::getSolo() const
{
    return m_solo;
}

void Track::loadFileIntoTransport(const juce::File& audioFile)
{
    m_audioFile = audioFile;

    std::unique_ptr<juce::AudioFormatReader> reader(m_formatManager.createReaderFor(m_audioFile));

    m_transportSource.setSource(nullptr);

    m_audioThumbnail.setSource(new juce::FileInputSource(m_audioFile));

    if (reader != nullptr)
    {
        m_currentAudioFileSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
        // ..and plug it into our transport source
        m_transportSource.setSource(m_currentAudioFileSource.get(),
            32768, // tells it to buffer this many samples ahead
            &m_thread, // this is the background thread to use for reading-ahead
            m_currentAudioFileSource->getAudioFormatReader()->sampleRate);
    }
    else
    {
        m_transportSource.setSource(nullptr);
        m_currentAudioFileSource.reset();
    }

    updateGain();

    m_duration = m_transportSource.getLengthInSeconds();
    m_durationChangedCallback();

    m_fileChangedCallback(m_audioFile, !m_loadingTrackConfig);

    if (!m_loadingTrackConfig)
        m_trackConfigChangedCallback();
}

void Track::reloadFile()
{
    loadFileIntoTransport(m_audioFile);
}

void Track::unloadFile()
{
    loadFileIntoTransport(juce::File());
}

void Track::timerCallback()
{
    const bool hasStreamFinished = m_transportSource.hasStreamFinished();

    if (hasStreamFinished)
    {
        stopTimer();
        m_playingStateChangedCallback(false);
    }

    callPositionCallbacks(m_transportSource.getCurrentPosition(), hasStreamFinished);
}

void Track::callPositionCallbacks(double position, bool hasStreamFinished)
{
    // Copy list so the callback can remove items and we continue to iterate.
    std::list<PositionCallback> positionCallbacks(m_positionCallbacks);

    for (const auto& callback : positionCallbacks)
        callback(position, hasStreamFinished);
}

void Track::saveToXml(juce::XmlElement* element) const
{
    element->setAttribute("stereo", m_stereo ? "true" : "false");
    element->setAttribute("mute", m_mute ? "true" : "false");
    element->setAttribute("solo", m_solo ? "true" : "false");
    element->setAttribute("pan", m_pan);
    element->setAttribute("gain", m_trackGain);

    juce::XmlElement* nameXml = new juce::XmlElement("Name");
    nameXml->addTextElement(getName());
    element->addChildElement(nameXml);

    element->addChildElement(m_remappingAudioSource.createXml());
}

void Track::restoreFromXml(const juce::XmlElement& element)
{
    m_stereo = element.getStringAttribute("stereo", "false") == "true";
    setMute(element.getStringAttribute("mute", "false") == "true");
    setSolo(element.getStringAttribute("solo", "false") == "true");
    setPan(element.getDoubleAttribute("pan", m_stereo ? 0.0f : NAN));
    setGain(static_cast<float>(element.getDoubleAttribute("gain", 1.0)));

    juce::XmlElement* nameXml = element.getChildByName("Name");
    if (nameXml != nullptr)
        setName(nameXml->getAllSubText().trim());

    // Element name is forced upon us by ChannelRemappingAudioSource from juce_ChannelRemappingAudioSource.cpp
    juce::XmlElement* mappingsXml = element.getChildByName("MAPPINGS");
    if (mappingsXml != nullptr)
        m_remappingAudioSource.restoreFromXml(*mappingsXml);
}

juce::AudioFormatManager& Track::getAudioFormatManager()
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

juce::AudioThumbnail& Track::getAudioThumbnail()
{
    return m_audioThumbnail;
}

void Track::soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel)
{
    juce::ignoreUnused(channel, outputChannel, previousOutputChannel);
    setSolo(m_solo);
}
