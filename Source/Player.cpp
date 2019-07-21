#include "Player.h"

#include "Track.h"
#include "TracksComponent.h"

using namespace InterPlayerCommunication;

Player::Player(MixerComponent* mixer, OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings,
    PlayerType type, ApplicationProperties& applicationProperties, AudioThumbnailCache& audioThumbnailCache,
    TimeSliceThread& thread, MTCSender& mtcSender, PluginLoader& pluginLoader, float gain, bool solo, bool mute)
    : m_mixer(mixer)
    , m_outputChannelNames(outputChannelNames)
    , m_soloBusSettings(soloBusSettings)
    , m_pluginLoader(pluginLoader)
    , m_gain(1.0f)
    , m_solo(solo)
    , m_soloMute(false)
    , m_mute(mute)
    , m_type(type)
    , m_tracksContainer(mixer, soloBusSettings, outputChannelNames->getNumberOfChannels(),
          std::bind(&Player::trackConfigChanged, this),
          std::bind(&Player::gainChangedCallback, this, std::placeholders::_1, std::placeholders::_2),
          audioThumbnailCache, thread, mtcSender)
    , m_playlistPlayer(*this, &m_tracksContainer, type == PlayerType::Playlist,
          std::bind(&Player::showEditDialog, this), std::bind(&Player::configureChannels, this),
          std::bind(&Player::configureMidi, this), std::bind(&Player::setType, this, std::placeholders::_1),
          playlistModel, applicationProperties)
    , m_jinglePlayer(*this, &m_tracksContainer, std::bind(&Player::showEditDialog, this),
          std::bind(&Player::configureChannels, this), std::bind(&Player::configureMidi, this),
          std::bind(&Player::setType, this, std::placeholders::_1),
          std::bind(&Player::setUserImage, this, std::placeholders::_1))
    , m_mtcSender(mtcSender)
{
    addChildComponent(&m_playlistPlayer);

    addChildComponent(&m_jinglePlayer);

    m_tracksContainer.addChannelCountChangedCallback([&]() {
        if (m_channelCountChanged)
            m_channelCountChanged();
    });

    mixer->registerPlayer(this);
    setGain(gain);
    setType(type);

    addKeyListener(this);
    setBounds(0, 0, 600, 300);
    Track::PlayingStateChangedCallback playingStateChangedCallback
        = [&](bool isPlaying) { m_pluginLoader.playingStateChanged(getName().toRawUTF8(), isPlaying); };
    m_tracksContainer.addPlayingStateChangedCallback(playingStateChangedCallback);

    m_soloBusSettings.addListener(this);

    Track::PositionCallback positionCallback = [&](double position, bool finished) {
        m_pluginLoader.positionChanged(getName().toRawUTF8(), position);
        if (finished && m_playlistPlayer.isVisible())
            nextEntry(true);
    };
    m_tracksContainer.addPositionCallback(positionCallback);
}

Player::~Player()
{
    m_mixer->unregisterPlayer(this);
    m_soloBusSettings.removeListener(this);
}

void Player::resized()
{
    m_playlistPlayer.setBounds(getLocalBounds());
    m_jinglePlayer.setBounds(getLocalBounds());
}

void Player::setType(PlayerType type)
{
    m_type = type;

    m_jinglePlayer.setVisible(type == PlayerType::Jingle);
    m_playlistPlayer.setVisible(type == PlayerType::Multitrack || type == PlayerType::Playlist);
    m_playlistPlayer.setShowPlaylist(type == PlayerType::Playlist);

    if (isShowing())
    {
        grabKeyboardFocus();
    }
}

void Player::setGain(float gain)
{
    m_tracksContainer.setGain(gain);
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::gainChanged, std::placeholders::_1, gain));
}

float Player::getGain() const
{
    return m_tracksContainer.getGain();
}

void Player::setPan(float pan)
{
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::panChanged, std::placeholders::_1, pan));
}

float Player::getPan() const
{
    return 1.0f;
}

void Player::setSoloMute(bool soloMute)
{
    m_soloMute = soloMute;
    updateGain();
}

bool Player::getSoloMute() const
{
    return m_soloMute;
}

void Player::setSolo(bool solo)
{
    m_solo = solo;
    updateGain();
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::soloChanged, std::placeholders::_1, solo));
    m_tracksContainer.setSolo(m_solo);
}

bool Player::getSolo() const
{
    return m_solo;
}

void Player::setMute(bool mute)
{
    m_mute = mute;
    updateGain();
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::muteChanged, std::placeholders::_1, mute));
}

bool Player::getMute() const
{
    return m_mute;
}

float Player::getVolume() const
{
    float maxVolume = 0;
    for (size_t i = 0; i < m_tracksContainer.size(); ++i)
        maxVolume = std::max(maxVolume, m_tracksContainer[i].getVolume());
    return maxVolume;
}

String Player::getName() const
{
    return Component::getName();
}

void Player::setName(const String& newName)
{
    Component::setName(newName);
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::nameChanged, std::placeholders::_1, newName));
}

void Player::updateGain()
{
    m_tracksContainer.setMute(m_mute || (!m_soloBusSettings.isConfigured() && m_soloMute && !m_solo));
}

void Player::setOutputChannels(int outputChannels)
{
    m_tracksContainer.setOutputChannels(outputChannels);
}

void Player::setColor(const Colour& color)
{
    m_color = color;

    m_mixer->updatePlayerColor(this, m_color);
    m_playlistPlayer.setColor(m_color);
    m_jinglePlayer.setColor(m_color);
}

void Player::setUserImage(const File& file)
{
    m_userImage = file;
    m_jinglePlayer.setUserImage(file);
}

XmlElement* Player::saveToXml(const File& projectDirectory, MyMultiDocumentPanel::LayoutMode layoutMode) const
{
    XmlElement* element = new XmlElement("Player");
    switch (m_type)
    {
    case PlayerType::Jingle:
        element->setAttribute("type", "jingle");
        break;
    case PlayerType::Multitrack:
        element->setAttribute("type", "multitrack");
        break;
    case PlayerType::Playlist:
        element->setAttribute("type", "playlist");
        break;
    }
    element->setAttribute("gain", m_tracksContainer.getGain());
    element->setAttribute("mtcEnabled", m_tracksContainer.getMtcEnabled());
    element->setAttribute("mute", m_mute);
    element->setAttribute("solo", m_solo);
    element->setAttribute("color", m_color.toString());
    if (m_userImage != File())
    {
        if (m_userImage.isAChildOf(projectDirectory))
            element->setAttribute("userImage", m_userImage.getRelativePathFrom(projectDirectory));
        else
            element->setAttribute("userImage", m_userImage.getFullPathName());
    }

    switch (layoutMode)
    {
    case MyMultiDocumentPanel::FloatingWindows:
    {
        Rectangle<int> parentBounds = getParentComponent()->getBounds();
        XmlElement* boundsXml = new XmlElement("Bounds");
        boundsXml->setAttribute("x", parentBounds.getX());
        boundsXml->setAttribute("y", parentBounds.getY());
        boundsXml->setAttribute("width", parentBounds.getWidth());
        boundsXml->setAttribute("height", parentBounds.getHeight());
        element->addChildElement(boundsXml);
    }
    break;
    case MyMultiDocumentPanel::MaximisedWindowsWithTabs:
    {
        XmlElement* mdiDocumentPosXml = new XmlElement("MdiDocumentPos");
        mdiDocumentPosXml->addTextElement(getProperties()["mdiDocumentPos_"]);
        element->addChildElement(mdiDocumentPosXml);
    }
    break;
    }

    XmlElement* viewXml = new XmlElement("View");
    viewXml->setAttribute("playlistResizerPosition", m_playlistPlayer.getResizerBarPosition());
    element->addChildElement(viewXml);

    XmlElement* nameXml = new XmlElement("Name");
    nameXml->addTextElement(Component::getName());
    element->addChildElement(nameXml);

    XmlElement* tracksXml = new XmlElement("Tracks");
    for (size_t i = 0; i < m_tracksContainer.size(); ++i)
    {
        XmlElement* trackElement = new XmlElement("Track");
        m_tracksContainer[i].saveToXml(trackElement);
        tracksXml->addChildElement(trackElement);
    }
    element->addChildElement(tracksXml);

    element->addChildElement(playlistModel.saveToXml(projectDirectory));

    return element;
}

void Player::restoreFromXml(const XmlElement& element, const File& projectDirectory)
{
    setColor(Colour::fromString(element.getStringAttribute("color", "0xffffffff")));
    if (element.hasAttribute("userImage"))
        setUserImage(projectDirectory.getChildFile(element.getStringAttribute("userImage")));
    repaint();

    m_tracksContainer.setMtcEnabled(element.getBoolAttribute("mtcEnabled"));

    XmlElement* boundsXml = element.getChildByName("Bounds");

    if (boundsXml)
    {
        String x = boundsXml->getStringAttribute("x", "0");
        String y = boundsXml->getStringAttribute("y", "0");
        String width = boundsXml->getStringAttribute("width", "150");
        String height = boundsXml->getStringAttribute("height", "150");
        getParentComponent()->setBounds(x.getIntValue(), y.getIntValue(), width.getIntValue(), height.getIntValue());
    }
    else
    {
        XmlElement* mdiDocumentPosXml = element.getChildByName("MdiDocumentPos");
        if (mdiDocumentPosXml->getNumChildElements() > 0 && mdiDocumentPosXml->getFirstChildElement()->isTextElement())
        {
            getProperties().set("mdiDocumentPos_", mdiDocumentPosXml->getFirstChildElement()->getText());
        }
    }

    XmlElement* viewXml = element.getChildByName("View");
    String playlistResizerPosition = viewXml->getStringAttribute("playlistResizerPosition", "100");
    m_playlistPlayer.setResizerBarPosition(playlistResizerPosition.getIntValue());

    XmlElement* nameXml = element.getChildByName("Name");
    setName(nameXml->getAllSubText().trim());

    XmlElement* tracksXml = element.getChildByName("Tracks");
    m_tracksContainer.clear();
    for (int i = 0; i < tracksXml->getNumChildElements(); ++i)
        m_tracksContainer.addTrack(false, tracksXml->getChildElement(i));

    XmlElement* playlistXml = element.getChildByName("Playlist");
    playlistModel.restoreFromXml(*playlistXml, projectDirectory);
}

void Player::SetChannelCountChangedCallback(const Track::ChannelCountChangedCallback& callback)
{
    m_channelCountChanged = callback;
}

std::vector<MixerControlable*> Player::getSubMixerControlables() const
{
    std::vector<MixerControlable*> controlables;
    for (size_t i = 0; i < m_tracksContainer.size(); ++i)
        controlables.push_back(&m_tracksContainer[i]);
    return controlables;
}

void Player::showEditDialog()
{
    if (!m_PlayerEditDialog)
    {
        m_PlayerEditDialog = std::make_unique<PlayerEditDialogWindow>(getName(),
            m_color,
            m_userImage.getFullPathName(),
            std::bind(&Player::setName, this, std::placeholders::_1),
            std::bind(&Player::setColor, this, std::placeholders::_1),
            [&]() {
                // clear is not working
                delete m_PlayerEditDialog.release();
            },
            std::bind(&Player::setUserImage, this, std::placeholders::_1));
    }
    m_PlayerEditDialog->addToDesktop();
    m_PlayerEditDialog->toFront(true);
}

void Player::configureChannels()
{
    if (!m_channelMappingWindow)
    {
        m_channelMappingWindow = std::make_unique<ChannelMappingWindow>(m_outputChannelNames,
            m_soloBusSettings,
            m_tracksContainer.createMapping(),
            [&](int source, int target) {
                for (size_t i = 0; i < m_tracksContainer.size(); ++i)
                {
                    if (source - m_tracksContainer[i].getNumChannels() < 0)
                    {
                        m_tracksContainer[i].setOutputChannelMapping(source, target);
                        break;
                    }
                    source -= m_tracksContainer[i].getNumChannels();
                }
            },
            [&]() {
                // clear is not working
                delete m_channelMappingWindow.release();
            });
    }
    m_channelMappingWindow->addToDesktop();
    m_channelMappingWindow->toFront(true);
}

void Player::configureMidi()
{
    if (!m_PlayerMidiDialog)
    {
        m_PlayerMidiDialog = std::make_unique<PlayerMidiDialogWindow>(m_tracksContainer.getMtcEnabled(),
            std::bind(&TracksContainer::setMtcEnabled, &m_tracksContainer, std::placeholders::_1),
            [&]() {
                // clear is not working
                delete m_PlayerMidiDialog.release();
            });
    }
    m_PlayerMidiDialog->addToDesktop();
    m_PlayerMidiDialog->toFront(true);
}

void Player::play()
{
    m_tracksContainer.play();
}

void Player::pause()
{
    m_tracksContainer.pause();
}

void Player::stop()
{
    m_tracksContainer.stop();
}

void Player::nextEntry(bool onlyIfEntrySaysSo)
{
    m_playlistPlayer.nextPlaylistEntry(onlyIfEntrySaysSo);
    m_pluginLoader.nextEntrySelected(getName().toRawUTF8());
}

void Player::previousEntry()
{
    m_playlistPlayer.previousPlaylistEntry();
    m_pluginLoader.previousEntrySelected(getName().toRawUTF8());
}

void Player::playlistEntryChanged(const std::vector<TrackConfig>& trackConfigs, bool play, int index)
{
    m_tracksContainer.setTrackConfigs(trackConfigs);
    m_pluginLoader.playlistEntrySelected(getName().toRawUTF8(), index);
    if (play)
        this->play();
}

void Player::gainChangedCallback(const char* track_name, float gain)
{
    m_pluginLoader.trackVolumeChanged(getName().toRawUTF8(), track_name, gain);
};

bool Player::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
    if (key == KeyPress::spaceKey)
    {
        if (m_tracksContainer.isPlaying())
            stop();
        else
            play();
        return true;
    }

    return false;
}

void Player::soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel)
{
    ignoreUnused(channel, outputChannel, previousOutputChannel);
    updateGain();
}

void Player::trackConfigChanged()
{
    playlistModel.setTrackConfigs(m_playlistPlayer.getSelectedRow(), m_tracksContainer.getTrackConfigs());
}