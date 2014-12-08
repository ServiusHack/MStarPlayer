#include "Player.h"

#include "Track.h"
#include "TracksComponent.h"

using namespace InterPlayerCommunication;

Player::Player(MixerComponent* mixer, OutputChannelNames *outputChannelNames, PlayerType type, ApplicationProperties& applicationProperties, float gain, bool solo, bool mute)
	: m_mixer(mixer)
	, m_outputChannelNames(outputChannelNames)
	, m_gain(1.0f)
	, m_solo(solo)
	, m_soloMute(false)
	, m_mute(mute)
	, m_type(type)
	, m_tracksContainer(mixer, outputChannelNames->getNumberOfChannels())
	, m_playlistPlayer(&m_tracksContainer, outputChannelNames, type == PlayerType::Playlist, 
		std::bind(&Player::showEditDialog,this),
		std::bind(&Player::configureChannels, this),
		std::bind(&Player::setType, this, std::placeholders::_1),
		playlistModel,
		applicationProperties)
	, m_jinglePlayer(&m_tracksContainer, outputChannelNames,
		std::bind(&Player::showEditDialog,this),
		std::bind(&Player::configureChannels, this),
		std::bind(&Player::setType, this, std::placeholders::_1))
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
}

Player::~Player()
{
	m_mixer->unregisterPlayer(this);
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
	grabKeyboardFocus();
}

void Player::setGain(float gain)
{
	m_tracksContainer.setGain(gain);
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->gainChanged(gain);
}

float Player::getGain() const
{
	return m_tracksContainer.getGain();
}

void Player::setPan(float pan)
{
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->panChanged(pan);
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
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->soloChanged(solo);
}

bool Player::getSolo() const
{
	return m_solo;
}

void Player::setMute(bool mute)
{
	m_mute = mute;
	updateGain();
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->muteChanged(mute);
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
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->nameChanged(newName);
}

void Player::updateGain()
{
	m_tracksContainer.setMute(m_mute || (m_soloMute && !m_solo));
}

void Player::setOutputChannels(int outputChannels)
{
	m_tracksContainer.setOutputChannels(outputChannels);
}

void Player::setColor(Colour color)
{
	m_color = color;

	m_mixer->updatePlayerColor(this, m_color);
	m_playlistPlayer.setColor(m_color);
	m_jinglePlayer.setColor(m_color);
}

XmlElement* Player::saveToXml() const
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
	element->setAttribute("mute", m_mute);
	element->setAttribute("solo", m_solo);
	element->setAttribute("color", m_color.toString());

	Rectangle<int> bounds = getParentComponent()->getBounds();

	XmlElement* boundsXml = new XmlElement("Bounds");
	boundsXml->setAttribute("x", bounds.getX());
	boundsXml->setAttribute("y", bounds.getY());
	boundsXml->setAttribute("width", bounds.getWidth());
	boundsXml->setAttribute("height", bounds.getHeight());
	element->addChildElement(boundsXml);

	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(Component::getName());
	element->addChildElement(nameXml);

	XmlElement* tracksXml = new XmlElement("Tracks");
	for (size_t i = 0; i < m_tracksContainer.size(); ++i)
		tracksXml->addChildElement(m_tracksContainer[i].saveToXml());
	element->addChildElement(tracksXml);

	element->addChildElement(playlistModel.saveToXml());

	return element;
}

void Player::restoreFromXml (const XmlElement& element)
{
	setColor(Colour::fromString(element.getStringAttribute("color", "0xffffffff")));
	repaint();

	XmlElement* boundsXml = element.getChildByName("Bounds");

	String x = boundsXml->getStringAttribute("x", "0");
	String y = boundsXml->getStringAttribute("y", "0");
	String width = boundsXml->getStringAttribute("width", "150");
	String height = boundsXml->getStringAttribute("height", "150");
	getParentComponent()->setBounds(x.getIntValue(), y.getIntValue(), width.getIntValue(), height.getIntValue());

	XmlElement* nameXml = element.getChildByName("Name");
	setName(nameXml->getAllSubText().trim());

	XmlElement* tracksXml = element.getChildByName("Tracks");
	m_tracksContainer.clear();
	for (int i = 0; i < tracksXml->getNumChildElements(); ++i)
		m_tracksContainer.addTrack(false, tracksXml->getChildElement(i));

	XmlElement* playlistXml = element.getChildByName("Playlist");
	playlistModel.restoreFromXml(*playlistXml);
}

void Player::SetChannelCountChangedCallback(Track::ChannelCountChangedCallback callback)
{
	m_channelCountChanged = callback;
}

std::vector<MixerControlable*> Player::getSubMixerControlables() const
{
	std::vector<MixerControlable*> controlables;
	for (size_t i = 0; i < m_tracksContainer.size(); ++i) {
		controlables.push_back(&m_tracksContainer[i]);
	}
	return controlables;
}

void Player::showEditDialog()
{
	if (m_PlayerEditDialog.get() == nullptr) {
		m_PlayerEditDialog.set(new PlayerEditDialogWindow(getName(), m_color, "",
			std::bind(&Player::setName, this, std::placeholders::_1),
			std::bind(&Player::setColor, this, std::placeholders::_1),
			[&]() {
				// clear is not working
				delete m_PlayerEditDialog.release();
			},
			std::bind(&JinglePlayerWindow::setUserImage, &m_jinglePlayer, std::placeholders::_1)), true);
	}
	m_PlayerEditDialog->addToDesktop();
	m_PlayerEditDialog->toFront(true);
}

void Player::configureChannels()
{
	if (m_channelMappingWindow.get() == nullptr) {
		m_channelMappingWindow.set(new ChannelMappingWindow(m_outputChannelNames, m_tracksContainer.createMapping(), [&](int source, int target) {
			m_tracksContainer[0].setOutputChannelMapping(source, target);
		}, [&]() {
			// clear is not working
			delete m_channelMappingWindow.release();
		}), true);
	}
	m_channelMappingWindow->addToDesktop();
	m_channelMappingWindow->toFront(true);
}

bool Player::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
	if (key == KeyPress::spaceKey)
	{
		m_tracksContainer.play();
		return true;
	}

	return false;
}