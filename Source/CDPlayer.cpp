#include "CDPlayer.h"

#include "Track.h"
#include "Utils.h"

CDPlayer::CDPlayer(MixerComponent* mixer, OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings,
    TimeSliceThread& thread, PluginLoader& pluginLoader, float gain, bool solo, bool mute)
    : m_mixer(mixer)
    , m_outputChannelNames(outputChannelNames)
    , m_soloBusSettings(soloBusSettings)
    , m_gain(1.0f)
    , m_solo(solo)
    , m_soloMute(false)
    , m_mute(mute)
    , m_pluginLoader(pluginLoader)
    , m_thread(thread)
    , m_playButton("Play")
    , m_pauseButton("Pause")
    , m_stopButton("Stop")
    , m_skipBackwardButton("Skip Backward")
    , m_skipForwardButton("Skip Forward")
    , m_configureButton("Configure")
#if JUCE_WINDOWS
    , m_ejectButton("Eject")
#endif
    , m_remappingAudioSource(&m_transportSource, soloBusSettings, false)
    , m_digitalDisplay(String::empty, "00:00:00")
    , m_slider(Slider::LinearHorizontal, Slider::NoTextBox)
    , m_tracksTable([&](int trackIndex) {
        setNextReadPosition(m_reader->getPositionOfTrackStart(trackIndex));
        m_transportSource.start();
        startTimer(50);
        m_pluginLoader.playingStateChanged(getName().toRawUTF8(), true);
    })
{
    // play button
    Image normalImage
        = ImageFileFormat::loadFrom(BinaryData::mediaplaybackstart_png, BinaryData::mediaplaybackstart_pngSize);
    m_playButton.addListener(this);
    m_playButton.setEnabled(false);
    m_playButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_playButton);

    // pause button
    m_pauseButton.addListener(this);
    m_pauseButton.setEnabled(false);
    normalImage = ImageFileFormat::loadFrom(BinaryData::mediaplaybackpause_png, BinaryData::mediaplaybackpause_pngSize);
    m_pauseButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_pauseButton);

    // stop button
    m_stopButton.addListener(this);
    m_stopButton.setEnabled(false);
    normalImage = ImageFileFormat::loadFrom(BinaryData::mediaplaybackstop_png, BinaryData::mediaplaybackstop_pngSize);
    m_stopButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_stopButton);

    // skip backward button
    m_skipBackwardButton.addListener(this);
    m_skipBackwardButton.setEnabled(false);
    normalImage = ImageFileFormat::loadFrom(BinaryData::mediaskipbackward_png, BinaryData::mediaskipbackward_pngSize);
    m_skipBackwardButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_skipBackwardButton);

    // skip forward button
    m_skipForwardButton.addListener(this);
    m_skipForwardButton.setEnabled(false);
    normalImage = ImageFileFormat::loadFrom(BinaryData::mediaskipforward_png, BinaryData::mediaskipforward_pngSize);
    m_skipForwardButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_skipForwardButton);

#if JUCE_WINDOWS
    // eject button
    m_ejectButton.addListener(this);
    m_ejectButton.setEnabled(false);
    normalImage = ImageFileFormat::loadFrom(BinaryData::mediaeject_png, BinaryData::mediaeject_pngSize);
    m_ejectButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_ejectButton);
#endif

    // configuration button
    normalImage = ImageFileFormat::loadFrom(BinaryData::configure_png, BinaryData::configure_pngSize);
    m_configureButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::transparentBlack,
        normalImage,
        1.0f,
        Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_configureButton);
    m_configureButton.addMouseListener(this, false);

    // playback time display
    addAndMakeVisible(m_digitalDisplay);

    // CD combo box
    m_availableCDsComboBox.addListener(this);
    addAndMakeVisible(m_availableCDsComboBox);

    addAndMakeVisible(m_slider);
    m_slider.addListener(this);
    m_slider.setEnabled(false);

    // playlist
    addAndMakeVisible(m_tracksTable);
    m_tracksTable.setColour(ListBox::outlineColourId, Colours::grey);
    m_tracksTable.setOutlineThickness(1);

    m_remappingAudioSource.setNumberOfChannelsToProduce(outputChannelNames->getNumberOfChannels());
    m_remappingAudioSource.setOutputChannelMapping(0, 0);
    m_remappingAudioSource.setOutputChannelMapping(1, 1);
    m_mixer->getMixerAudioSource().addInputSource(&m_remappingAudioSource, false);

    mixer->registerPlayer(this);
    setGain(gain);

    addKeyListener(this);
    setBounds(0, 0, 600, 300);

    m_soloBusSettings.addListener(this);
}

CDPlayer::~CDPlayer()
{
    m_mixer->getMixerAudioSource().removeInputSource(&m_remappingAudioSource);
    m_mixer->unregisterPlayer(this);
    m_soloBusSettings.removeListener(this);
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

#define PLACE_BUTTON(IDX, BTN) BTN.setBounds(IDX* buttonWidth + 3, 3, buttonWidth - 6, buttonHeight - 6);
    PLACE_BUTTON(0, m_playButton);
    PLACE_BUTTON(1, m_pauseButton);
    PLACE_BUTTON(2, m_stopButton);
    PLACE_BUTTON(3, m_skipBackwardButton);
    PLACE_BUTTON(4, m_skipForwardButton);
#if JUCE_WINDOWS
    PLACE_BUTTON(5, m_ejectButton);
    PLACE_BUTTON(6, m_configureButton);
#else
    PLACE_BUTTON(5, m_configureButton);
#endif
#undef PLACE_BUTTON
    m_digitalDisplay.setBounds(7 * buttonWidth + 3, 3, buttonWidth * 3, buttonHeight - 6);
    m_availableCDsComboBox.setBounds(10 * buttonWidth + 3, 3, getWidth() - (10 * buttonWidth + 3), buttonHeight - 6);

    m_slider.setBounds(0, buttonHeight, getWidth(), buttonHeight);
    m_tracksTable.setBounds(0, buttonHeight * 2, getWidth(), getHeight() - buttonHeight * 2);
}

void CDPlayer::mouseDown(const MouseEvent& event)
{
    if (event.eventComponent != &m_configureButton)
        return;

    PopupMenu m;
    m.addItem(1, TRANS("configure channels"));
    m.addItem(2, TRANS("configure appearance"));
    const int result = m.show();

    switch (result)
    {
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
    m_gain = gain;
    updateGain();
    m_pluginLoader.trackVolumeChanged(getName().toRawUTF8(), "", m_gain);
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::gainChanged, std::placeholders::_1, gain));
}

float CDPlayer::getGain() const
{
    return m_gain;
}

void CDPlayer::setPan(float /*pan*/) {}

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
    if (m_soloBusSettings.isConfigured())
        m_remappingAudioSource.setSolo(solo);
    else
        updateGain();
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::soloChanged, std::placeholders::_1, solo));
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
    std::for_each(m_listeners.begin(),
        m_listeners.end(),
        std::bind(&MixerControlableChangeListener::nameChanged, std::placeholders::_1, newName));
}

void CDPlayer::SetChannelCountChangedCallback(const Track::ChannelCountChangedCallback& /*callback*/) {}

void CDPlayer::updateGain()
{
    const bool mute = m_mute || (!m_soloBusSettings.isConfigured() && m_soloMute && !m_solo);
    m_transportSource.setGain(mute ? 0.0f : m_gain);
}

void CDPlayer::setColor(const Colour& color)
{
    m_color = color;

    m_mixer->updatePlayerColor(this, m_color);
    m_tracksTable.setColour(ListBox::backgroundColourId, m_color);
    m_digitalDisplay.setColour(Label::textColourId, m_color.isTransparent() ? Colours::black : m_color.contrasting());
    m_availableCDsComboBox.setColour(Label::backgroundColourId, m_color);
    repaint();
}

XmlElement* CDPlayer::saveToXml(const File& /*projectDirectory*/, MyMultiDocumentPanel::LayoutMode layoutMode) const
{
    XmlElement* element = new XmlElement("CDPlayer");
    element->setAttribute("gain", getGain());
    element->setAttribute("mute", m_mute);
    element->setAttribute("solo", m_solo);
    element->setAttribute("color", m_color.toString());

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

    XmlElement* nameXml = new XmlElement("Name");
    nameXml->addTextElement(Component::getName());
    element->addChildElement(nameXml);

    XmlElement* driveXml = new XmlElement("Drive");
    driveXml->addTextElement(m_availableCDsComboBox.getText());
    element->addChildElement(driveXml);

    return element;
}

void CDPlayer::restoreFromXml(const XmlElement& element, const File& /*projectDirectory*/)
{
    setColor(Colour::fromString(element.getStringAttribute("color", "0xffffffff")));
    repaint();

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

    XmlElement* nameXml = element.getChildByName("Name");
    setName(nameXml->getAllSubText().trim());

    XmlElement* driveXml = element.getChildByName("Drive");
    m_availableCDsComboBox.selectDrive(driveXml->getAllSubText().trim());
}

std::vector<MixerControlable*> CDPlayer::getSubMixerControlables() const
{
    std::vector<MixerControlable*> controlables;
    return controlables;
}

void CDPlayer::showEditDialog()
{
    if (!m_PlayerEditDialog)
    {
        m_PlayerEditDialog = std::make_unique<PlayerEditDialogWindow>(getName(),
            m_color,
            String(),
            std::bind(&CDPlayer::setName, this, std::placeholders::_1),
            std::bind(&CDPlayer::setColor, this, std::placeholders::_1),
            [&]() {
                // clear is not working
                delete m_PlayerEditDialog.release();
            },
            PlayerEditDialogWindow::ImageChangedCallback());
    }
    m_PlayerEditDialog->addToDesktop();
    m_PlayerEditDialog->toFront(true);
}

std::vector<std::pair<char, int>> CDPlayer::createMapping()
{
    std::vector<std::pair<char, int>> mapping{{'l', m_remappingAudioSource.getRemappedOutputChannel(0)},
        {'r', m_remappingAudioSource.getRemappedOutputChannel(1)}};
    return mapping;
}

void CDPlayer::configureChannels()
{
    if (m_channelMappingWindow)
    {
        m_channelMappingWindow = std::make_unique<ChannelMappingWindow>(m_outputChannelNames,
            m_soloBusSettings,
            createMapping(),
            [&](int source, int target) { m_remappingAudioSource.setOutputChannelMapping(source, target); },
            [&]() {
                // clear is not working
                delete m_channelMappingWindow.release();
            });
    }
    m_channelMappingWindow->addToDesktop();
    m_channelMappingWindow->toFront(true);
}

void CDPlayer::setNextReadPosition(int64 sampleInCDSampleRate)
{
    int64 sampleInPlaybackSampleRate
        = static_cast<int64>(sampleInCDSampleRate * m_remappingAudioSource.getSampleRate() / m_reader->sampleRate);
    m_transportSource.setNextReadPosition(sampleInPlaybackSampleRate);
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
    if (button == &m_playButton)
    {
        m_transportSource.start();
        startTimer(50);
        m_pluginLoader.playingStateChanged(getName().toRawUTF8(), true);
    }
    else if (button == &m_pauseButton)
    {
        if (m_transportSource.isPlaying())
        {
            m_transportSource.stop();
            stopTimer();
            m_pluginLoader.playingStateChanged(getName().toRawUTF8(), false);
        }
        else
        {
            m_transportSource.start();
            startTimer(50);
            m_pluginLoader.playingStateChanged(getName().toRawUTF8(), true);
        }
    }
    else if (button == &m_stopButton)
    {
        m_transportSource.setPosition(0);
        m_transportSource.stop();
        stopTimer();
        m_pluginLoader.playingStateChanged(getName().toRawUTF8(), false);
    }
#if JUCE_WINDOWS
    else if (button == &m_ejectButton)
    {
        m_transportSource.stop();
        stopTimer();
        m_pluginLoader.playingStateChanged(getName().toRawUTF8(), false);
        m_reader->ejectDisk();
    }
#endif
    else if (button == &m_skipBackwardButton)
    {
        m_tracksTable.previous();
    }
    else if (button == &m_skipForwardButton)
    {
        m_tracksTable.next();
    }
}

void CDPlayer::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    std::unique_ptr<AudioCDReader> newReader(
        AudioCDReader::createReaderForCD(comboBoxThatHasChanged->getSelectedItemIndex()));

    // Update UI state.
    const bool cdPresent = newReader && newReader->isCDStillPresent();
    m_playButton.setEnabled(cdPresent);
    m_pauseButton.setEnabled(cdPresent);
    m_stopButton.setEnabled(cdPresent);
    m_skipBackwardButton.setEnabled(cdPresent);
    m_skipForwardButton.setEnabled(cdPresent);
#if JUCE_WINDOWS
    m_ejectButton.setEnabled(comboBoxThatHasChanged->getSelectedItemIndex() != -1);
#endif

    // Clear out objects for previous CD.
    m_tracksTable.setModel(nullptr);
    m_model.release();
    m_source = nullptr;
    m_reader.release();

    // Create new objects if we have a CD.
    if (newReader)
    {
        newReader->refreshTrackLengths();

        m_source = std::make_unique<AudioFormatReaderSource>(newReader.get(), false);

        m_transportSource.setSource(m_source.get(),
            32768, // tells it to buffer this many samples ahead
            &m_thread, // this is the background thread to use for reading-ahead
            newReader->sampleRate);

        {
            std::unique_ptr<CDTracksModel> model = std::make_unique<CDTracksModel>(*newReader.get());
            m_tracksTable.setModel(model.get());
            m_model = std::move(model);
        }

        m_reader = std::move(newReader);
    }
}

void CDPlayer::sliderValueChanged(Slider* sliderThatWasMoved)
{
    // Possible loss of precision is acceptable for very large values because user can't select specific value that
    // precise.
    setNextReadPosition(static_cast<int64>(sliderThatWasMoved->getValue()));
}

void CDPlayer::soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel)
{
    ignoreUnused(channel, outputChannel, previousOutputChannel);
    updateGain();
}

void CDPlayer::timerCallback()
{
    m_digitalDisplay.setText(Utils::formatSeconds(m_transportSource.getCurrentPosition()), sendNotification);

    int64 currentSample = m_source->getNextReadPosition();

    const Array<int>& offsets = m_reader->getTrackOffsets();
    // offsets[0] != 0 is possible, there might be a track before the first.
    // offsets.getLast() is not the start of the last track but the end sample of the CD.

    if (currentSample < offsets[0])
    {
        // We are before the first track.
        m_slider.setEnabled(false);
        m_slider.setValue(m_slider.getMinimum(), juce::dontSendNotification);
        m_currentTrack = -1;
        SparseSet<int> rows;
        m_tracksTable.setSelectedRows(rows, juce::dontSendNotification);
        return;
    }

    if (m_currentTrack == -1)
    {
        // We have now entered the first track.
        m_slider.setEnabled(true);
        m_slider.setRange(offsets[0], offsets[1]);
        // Possible loss of precision is acceptable for very large values because user can't select specific value that
        // precise.
        m_slider.setValue(static_cast<double>(currentSample), juce::dontSendNotification);
        m_currentTrack = 0;
        m_pluginLoader.playlistEntrySelected(getName().toRawUTF8(), m_currentTrack);
        SparseSet<int> rows;
        rows.addRange(Range<int>(m_currentTrack, m_currentTrack + 1));
        m_tracksTable.setSelectedRows(rows, juce::dontSendNotification);
        return;
    }

    jassert(m_currentTrack + 1 < offsets.size());

    if (currentSample >= offsets[m_currentTrack] && currentSample < offsets[m_currentTrack + 1])
    {
        // We are still within the current track (usual case).
        // Possible loss of precision is acceptable for very large values because user can't select specific value that
        // precise.
        m_slider.setValue(static_cast<double>(currentSample), juce::dontSendNotification);
        return;
    }

    // We are within another track.

    int64 firstSample = -1;
    int64 lastSample = -1;

    if (currentSample < offsets[m_currentTrack])
    {
        // We are before the curren track. Search backward to find the track our sample position now lies within.

        // Assume our current track is the immediately preceeding track.
        jassert(m_currentTrack - 1 >= 0);
        firstSample = -1;
        lastSample = offsets[m_currentTrack];

        for (int probeTrack = m_currentTrack - 1; probeTrack >= 0; --probeTrack)
        {
            if (currentSample >= offsets[probeTrack])
            {
                m_currentTrack = probeTrack;
                firstSample = offsets[probeTrack];
                break; // We are at the previous track.
            }

            lastSample = offsets[probeTrack];
        }

        m_pluginLoader.previousEntrySelected(getName().toRawUTF8());
        m_pluginLoader.playlistEntrySelected(getName().toRawUTF8(), m_currentTrack);
    }

    if (currentSample >= offsets[m_currentTrack + 1])
    {
        // We are after the curren track. Search forward to find the track our sample position now lies within.

        // Assume our current track is the immediately following track.
        jassert(m_currentTrack + 2 < offsets.size());
        firstSample = offsets[m_currentTrack + 1];
        lastSample = -1;

        for (int probeTrack = m_currentTrack + 2; probeTrack < offsets.size(); ++probeTrack)
        {
            if (offsets[probeTrack] > currentSample)
            {
                m_currentTrack = probeTrack - 1;
                lastSample = offsets[probeTrack];
                break; // We are at the next track.
            }

            firstSample = offsets[probeTrack];
        }

        m_pluginLoader.nextEntrySelected(getName().toRawUTF8());
        m_pluginLoader.playlistEntrySelected(getName().toRawUTF8(), m_currentTrack);
    }

    jassert(currentSample >= firstSample);
    jassert(currentSample < lastSample);

    // Possible loss of precision is acceptable for very large values because user can't select specific value that
    // precise.
    m_slider.setRange(static_cast<double>(firstSample), static_cast<double>(lastSample));
    m_slider.setValue(static_cast<double>(currentSample), juce::dontSendNotification);
    SparseSet<int> rows;
    rows.addRange(Range<int>(m_currentTrack, m_currentTrack + 1));
    m_tracksTable.setSelectedRows(rows, juce::dontSendNotification);
    m_pluginLoader.positionChanged(getName().toRawUTF8(), m_transportSource.getCurrentPosition());
}
