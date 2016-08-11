#include "CDPlayer.h"

#include "Track.h"
#include "Utils.h"

CDPlayer::CDPlayer(MixerComponent* mixer, OutputChannelNames *outputChannelNames, float gain, bool solo, bool mute)
    : m_mixer(mixer)
    , m_outputChannelNames(outputChannelNames)
    , m_gain(1.0f)
    , m_solo(solo)
    , m_soloMute(false)
    , m_mute(mute)
    , m_thread("track")
    , m_remappingAudioSource(&m_transportSource, false)
{
    // play button
    m_playButton = new ImageButton("Play");
    Image normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackstart_png, BinaryData::mediaplaybackstart_pngSize);
    m_playButton->addListener(this);
    m_playButton->setEnabled(false);
    m_playButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_playButton);

    // pause button
    m_pauseButton = new ImageButton("Pause");
    m_pauseButton->addListener(this);
    m_pauseButton->setEnabled(false);
    normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackpause_png, BinaryData::mediaplaybackpause_pngSize);
    m_pauseButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_pauseButton);

    // stop button
    m_stopButton = new ImageButton("Stop");
    m_stopButton->addListener(this);
    m_stopButton->setEnabled(false);
    normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackstop_png, BinaryData::mediaplaybackstop_pngSize);
    m_stopButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_stopButton);

    // skip backward button
    m_skipBackwardButton = new ImageButton("Skip Backward");
    m_skipBackwardButton->addListener(this);
    m_skipBackwardButton->setEnabled(false);
    normalImage = ImageFileFormat::loadFrom (BinaryData::mediaskipbackward_png, BinaryData::mediaskipbackward_pngSize);
    m_skipBackwardButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_skipBackwardButton);

    // skip forward button
    m_skipForwardButton = new ImageButton("Skip Forward");
    m_skipForwardButton->addListener(this);
    m_skipForwardButton->setEnabled(false);
    normalImage = ImageFileFormat::loadFrom (BinaryData::mediaskipforward_png, BinaryData::mediaskipforward_pngSize);
    m_skipForwardButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_skipForwardButton);

    // configuration button
    m_configureButton = new ImageButton("Configure");
    m_configureButton->addListener(this);
    normalImage = ImageFileFormat::loadFrom (BinaryData::configure_png, BinaryData::configure_pngSize);
    m_configureButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_configureButton);

    // eject button
    m_ejectButton = new ImageButton("Eject");
    m_ejectButton->addListener(this);
    m_ejectButton->setEnabled(false);
    normalImage = ImageFileFormat::loadFrom (BinaryData::mediaeject_png, BinaryData::mediaeject_pngSize);
    m_ejectButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_ejectButton);

    // configuration button
    m_configureButton = new ImageButton("Configure");
    normalImage = ImageFileFormat::loadFrom (BinaryData::configure_png, BinaryData::configure_pngSize);
    m_configureButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
    addAndMakeVisible(m_configureButton);
    m_configureButton->addMouseListener(this, false);

    // playback time display
    addAndMakeVisible(m_digitalDisplay = new Label(String::empty, "00:00:00"));

    // CD combo box
    m_availableCDsComboBox = new CDNamesComboBox();
    m_availableCDsComboBox->addListener(this);
    addAndMakeVisible(m_availableCDsComboBox);

    // playlist
    CDTracksTable::TrackChangedCallback playlistCallback = [&](int trackIndex) {
        m_transportSource.setNextReadPosition(m_reader->getPositionOfTrackStart(trackIndex));
        m_transportSource.start();
        startTimer(50);
    };
    addAndMakeVisible(m_tracksTable = new CDTracksTable(playlistCallback));
    m_tracksTable->setColour(ListBox::outlineColourId, Colours::grey);
    m_tracksTable->setOutlineThickness(1);

    m_remappingAudioSource.setNumberOfChannelsToProduce(outputChannelNames->getNumberOfChannels());
    m_remappingAudioSource.setOutputChannelMapping(0, 0);
    m_remappingAudioSource.setOutputChannelMapping(1, 1);
    m_mixer->getMixerAudioSource().addInputSource(&m_remappingAudioSource, false);

    mixer->registerPlayer(this);
    setGain(gain);

    addKeyListener(this);
    setBounds(0, 0, 600, 300);

    m_thread.startThread(3);
}

CDPlayer::~CDPlayer()
{
    m_mixer->getMixerAudioSource().removeInputSource(&m_remappingAudioSource);
    m_mixer->unregisterPlayer(this);
}

void CDPlayer::paint(Graphics& g)
{
    g.fillAll(m_color);

    Component::paint(g);
}

void CDPlayer::resized()
{
    int buttonWidth = std::min(getWidth() / 11, 32);
    int buttonHeight = buttonWidth;

#define PLACE_BUTTON(IDX,BTN) BTN->setBounds(IDX * buttonWidth + 3, 3, buttonWidth - 6, buttonHeight - 6);
    PLACE_BUTTON(0, m_playButton);
    PLACE_BUTTON(1, m_pauseButton);
    PLACE_BUTTON(2, m_stopButton);
    PLACE_BUTTON(3, m_skipBackwardButton);
    PLACE_BUTTON(4, m_skipForwardButton);
    PLACE_BUTTON(5, m_ejectButton);
    PLACE_BUTTON(6, m_configureButton);
#undef PLACE_BUTTON
    m_digitalDisplay->setBounds(7 * buttonWidth + 3, 3, buttonWidth * 3, buttonHeight - 6);
    m_availableCDsComboBox->setBounds(10 * buttonWidth + 3, 3, getWidth() - (10 * buttonWidth + 3), buttonHeight - 6);

    m_tracksTable->setBounds(0, buttonHeight, getWidth(), getHeight() - buttonHeight);
}

void CDPlayer::mouseDown(const MouseEvent & event)
{
    if (event.eventComponent != m_configureButton)
        return;

    PopupMenu m;
    m.addItem (1, "configure channels");
    m.addItem (2, "configure appearance");
    const int result = m.show();

    switch (result) {
    case 1:
        configureChannels();
        break;
    case 2:
        showEditDialog();
        break;
    }

}

void CDPlayer::setGain(float gain)
{
    std::for_each(m_listeners.begin(), m_listeners.end(), std::bind(&MixerControlableChangeListener::gainChanged, std::placeholders::_1, gain));
}

float CDPlayer::getGain() const
{
    return 0.0f;
}

void CDPlayer::setPan(float /*pan*/)
{
}

float CDPlayer::getPan() const
{
    return 1.0f;
}

void CDPlayer::setSoloMute(bool soloMute)
{
    m_soloMute = soloMute;
    updateGain();
}

bool CDPlayer::getSoloMute() const
{
    return m_soloMute;
}

void CDPlayer::setSolo(bool solo)
{
    m_solo = solo;
    updateGain();
}

bool CDPlayer::getSolo() const
{
    return m_solo;
}

void CDPlayer::setMute(bool mute)
{
    m_mute = mute;
    updateGain();
}

bool CDPlayer::getMute() const
{
    return m_mute;
}

float CDPlayer::getVolume() const
{
    return m_remappingAudioSource.getVolume();
}

String CDPlayer::getName() const
{
    return Component::getName();
}

void CDPlayer::setName(const String& newName)
{
    Component::setName(newName);
}

void CDPlayer::SetChannelCountChangedCallback(const Track::ChannelCountChangedCallback& /*callback*/)
{
}

void CDPlayer::updateGain()
{
    bool mute =  m_mute || (m_soloMute && !m_solo);
    m_transportSource.setGain(mute ? 0.0f : m_gain);
}

void CDPlayer::setColor(const Colour& color)
{
    m_color = color;

    m_mixer->updatePlayerColor(this, m_color);
    m_tracksTable->setColour(ListBox::backgroundColourId, m_color);
    m_digitalDisplay->setColour(Label::textColourId, m_color.contrasting());
    m_availableCDsComboBox->setColour(Label::backgroundColourId, m_color);
    repaint();
}

XmlElement* CDPlayer::saveToXml(const File& /*projectDirectory*/) const
{
    XmlElement* element = new XmlElement("CDPlayer");
    element->setAttribute("gain", getGain());
    element->setAttribute("mute", m_mute);
    element->setAttribute("solo", m_solo);
    element->setAttribute("color", m_color.toString());

    Rectangle<int> parentBounds = getParentComponent()->getBounds();

    XmlElement* boundsXml = new XmlElement("Bounds");
    boundsXml->setAttribute("x", parentBounds.getX());
    boundsXml->setAttribute("y", parentBounds.getY());
    boundsXml->setAttribute("width", parentBounds.getWidth());
    boundsXml->setAttribute("height", parentBounds.getHeight());
    element->addChildElement(boundsXml);

    XmlElement* nameXml = new XmlElement("Name");
    nameXml->addTextElement(Component::getName());
    element->addChildElement(nameXml);

    return element;
}

void CDPlayer::restoreFromXml (const XmlElement& element, const File& /*projectDirectory*/)
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
}

std::vector<MixerControlable*> CDPlayer::getSubMixerControlables() const
{
    std::vector<MixerControlable*> controlables;
    return controlables;
}

void CDPlayer::showEditDialog()
{
    if (m_PlayerEditDialog.get() == nullptr) {
        m_PlayerEditDialog.set(new PlayerEditDialogWindow(getName(), m_color, String(),
            std::bind(&CDPlayer::setName, this, std::placeholders::_1),
            std::bind(&CDPlayer::setColor, this, std::placeholders::_1),
            [&]() {
                // clear is not working
                delete m_PlayerEditDialog.release();
            },
            PlayerEditDialogWindow::ImageChangedCallback()), true);
    }
    m_PlayerEditDialog->addToDesktop();
    m_PlayerEditDialog->toFront(true);

}

std::vector<std::pair<char, int>> CDPlayer::createMapping()
{
    std::vector<std::pair<char, int>> mapping{
        { 'l', m_remappingAudioSource.getRemappedOutputChannel(0) },
        { 'r', m_remappingAudioSource.getRemappedOutputChannel(1) }
    };
    return mapping;
}

void CDPlayer::configureChannels()
{
    if (m_channelMappingWindow.get() == nullptr) {
        m_channelMappingWindow.set(new ChannelMappingWindow(m_outputChannelNames, createMapping(), [&](int source, int target) {
            m_remappingAudioSource.setOutputChannelMapping(source, target);
        }, [&]() {
            // clear is not working
            delete m_channelMappingWindow.release();
        }), true);
    }
    m_channelMappingWindow->addToDesktop();
    m_channelMappingWindow->toFront(true);

}

bool CDPlayer::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
    if (key == KeyPress::spaceKey)
    {
        return true;
    }

    return false;
}

void CDPlayer::buttonClicked(Button* button)
{
    if (button == m_playButton)
    {
        m_transportSource.start();
        startTimer(50);
    }
    else if (button == m_pauseButton)
    {
        if (m_transportSource.isPlaying()) {
            m_transportSource.stop();
            stopTimer();
        }
        else {
            m_transportSource.start();
            startTimer(50);
        }
    }
    else if (button == m_stopButton)
    {
        m_transportSource.setPosition(0);
        m_transportSource.stop();
        stopTimer();
    }
    else if (button == m_ejectButton)
    {
        m_reader->ejectDisk();
    }
    else if (button == m_skipBackwardButton)
    {
        m_tracksTable->previous();
    }
    else if (button == m_skipForwardButton)
    {
        m_tracksTable->next();
    }
}

void CDPlayer::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    OptionalScopedPointer<AudioCDReader> newReader(AudioCDReader::createReaderForCD(comboBoxThatHasChanged->getSelectedItemIndex()), true);

    // Update UI state.
    const bool cdPresent = newReader && newReader->isCDStillPresent();
    m_playButton->setEnabled(cdPresent);
    m_pauseButton->setEnabled(cdPresent);
    m_stopButton->setEnabled(cdPresent);
    m_skipBackwardButton->setEnabled(cdPresent);
    m_skipForwardButton->setEnabled(cdPresent);
    m_ejectButton->setEnabled(comboBoxThatHasChanged->getSelectedItemIndex() != -1);

    // Clear out objects for previous CD.
    m_tracksTable->setModel(nullptr);
    m_model.release();
    m_source.release();
    m_reader.release();

    // Create new objects if we have a CD.
    if (newReader)
    {
        newReader->refreshTrackLengths();

        m_source.setOwned(new AudioFormatReaderSource(newReader.get(), false));

        m_transportSource.setSource(m_source.get(),
                                    32768, // tells it to buffer this many samples ahead
                                    &m_thread, // this is the background thread to use for reading-ahead
                                    newReader->sampleRate);

        {
            OptionalScopedPointer<CDTracksModel> model(new CDTracksModel(*newReader.get()), true);
            m_tracksTable->setModel(model.get());
            m_model = model;
        }

        m_reader = newReader;
    }
}

void CDPlayer::timerCallback()
{
    m_digitalDisplay->setText(Utils::formatSeconds(m_transportSource.getCurrentPosition()), sendNotification);
}
