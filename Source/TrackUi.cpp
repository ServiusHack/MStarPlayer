#include "TrackUi.h"

#include <sstream>

TrackUi::TrackUi(Track& track, ApplicationProperties& applicationProperties, SetPositionCallback setPositionCallback,
    RemoveTrackCallback removeTrackCallback, TrackHasFilesCallback trackHasFilesCallback,
    FileLoadedCallback fileLoadedCallback)
    : Component("TrackUi")
    , m_track(track)
    , m_longestDuration(0)
    , m_progress(0)
    , m_applicationProperties(applicationProperties)
    , m_setPositionCallback(setPositionCallback)
    , m_removeTrackCallback(removeTrackCallback)
    , m_trackHasFilesCallback(trackHasFilesCallback)
    , m_fileLoadedCallback(fileLoadedCallback)
    , m_editButton("edit")
    , m_soloButton("solo")
    , m_muteButton("mute")
    , m_fileNameLabel("filename label")
{
    addAndMakeVisible(m_waveform);

    addAndMakeVisible(m_idLabel);
    updateIdText();

    addAndMakeVisible(m_descriptionLabel);
    m_descriptionLabel.setText(getName(), sendNotification);
    m_descriptionLabel.setJustificationType(Justification::topLeft);

    addAndMakeVisible(m_volumeSlider);
    m_volumeSlider.setValue(1.0);
    m_volumeSlider.addListener(this);

    Image editImage = ImageFileFormat::loadFrom(BinaryData::configure_png, BinaryData::configure_pngSize);
    m_editButton.setImages(true,
        true,
        true,
        editImage,
        0.7f,
        Colours::transparentBlack,
        editImage,
        1.0f,
        Colours::transparentBlack,
        editImage,
        1.0f,
        Colours::transparentBlack,
        0.0f);
    addAndMakeVisible(m_editButton);
    m_editButton.addListener(this);

    Image soloImage = ImageFileFormat::loadFrom(BinaryData::audioheadphones_png, BinaryData::audioheadphones_pngSize);
    m_soloButton.setClickingTogglesState(true);
    m_soloButton.addListener(this);
    m_soloButton.setImages(true,
        true,
        true,
        soloImage,
        0.7f,
        Colours::transparentBlack,
        soloImage,
        1.0f,
        Colours::transparentBlack,
        soloImage,
        1.0f,
        Colours::red.withAlpha(0.5f),
        0.0f);
    addAndMakeVisible(m_soloButton);

    Image muteedImage
        = ImageFileFormat::loadFrom(BinaryData::audiovolumemuted_png, BinaryData::audiovolumemuted_pngSize);
    Image unmutedImage
        = ImageFileFormat::loadFrom(BinaryData::audiovolumemedium_png, BinaryData::audiovolumemedium_pngSize);
    m_muteButton.setImages(true,
        true,
        true,
        unmutedImage,
        0.7f,
        Colours::transparentBlack,
        unmutedImage,
        1.0f,
        Colours::transparentBlack,
        muteedImage,
        1.0f,
        Colours::transparentBlack,
        0.0f);
    m_muteButton.setClickingTogglesState(true);
    m_muteButton.addListener(this);
    addAndMakeVisible(m_muteButton);

    m_fileNameLabel.setColour(Label::backgroundColourId, Colour(0xccffffff));
    addAndMakeVisible(m_fileNameLabel);

    m_track.addChangeListener(this);
    m_track.getAudioThumbnail().addChangeListener(this);
    m_track.addPositionCallback(std::bind(&TrackUi::positionChanged, this, std::placeholders::_1));
    m_track.setFileChangedCallback(
        std::bind(&TrackUi::fileChanged, this, std::placeholders::_1, std::placeholders::_2));
}

TrackUi::~TrackUi()
{
    m_track.removeChangeListener(this);
    m_volumeSlider.removeListener(this);
}

void TrackUi::gainChanged(float gain)
{
    m_volumeSlider.setValue(gain);
}

void TrackUi::muteChanged(bool mute)
{
    m_muteButton.setToggleState(mute, sendNotification);
}

void TrackUi::soloChanged(bool solo)
{
    m_soloButton.setToggleState(solo, sendNotification);
}

void TrackUi::nameChanged(const String& name)
{
    m_descriptionLabel.setText(name, sendNotification);
}

void TrackUi::buttonClicked(Button* button)
{
    if (button == &m_muteButton)
    {
        m_track.setMute(m_muteButton.getToggleState());
    }
    else if (button == &m_soloButton)
    {
        m_track.setSolo(m_soloButton.getToggleState());
    }
    else if (button == &m_editButton)
    {
        PopupMenu m;
        m.addItem(1, TRANS("edit track"));
        m.addItem(5, TRANS("delete track"), !m_trackHasFilesCallback(m_track.getTrackIndex()));
        m.addItem(2, TRANS("open file"));
        m.addItem(3, TRANS("edit file"), m_track.getTrackConfig().file != File());
        m.addItem(4, TRANS("remove file"), m_track.getTrackConfig().file != File());
        m.addSeparator();
        const int result = m.show();

        switch (result)
        {
        case 1:
            m_editDialog = std::make_unique<TrackEditDialogWindow>(m_track.getName(),
                m_track.getGain(),
                std::bind(&Track::setName, &m_track, std::placeholders::_1),
                std::bind(&Track::setGain, &m_track, std::placeholders::_1));
            break;
        case 2:
            loadFile();
            break;
        case 3:
        {
            if (!File(m_applicationProperties.getUserSettings()->getValue("audioEditor")).existsAsFile())
            {
                AlertWindow::showMessageBox(
                    AlertWindow::WarningIcon, TRANS("No audio editor"), TRANS("No audio editor was configured."));
                return;
            }

            ChildProcess process;
            StringArray arguments(m_applicationProperties.getUserSettings()->getValue("audioEditor"));
            arguments.add(m_track.getTrackConfig().file.getFullPathName());
            if (!process.start(arguments))
            {
                AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                    TRANS("Failed launch audio editor"),
                    TRANS("Failed to start the audio editor."));
                return;
            }

            File file(m_track.getTrackConfig().file);
            m_track.unloadFile();
            AlertWindow blockDialog(TRANS("Audio editor launched"),
                TRANS("Modify the file in the audio editor. Click on 'ok' after the file was saved to load it again."),
                AlertWindow::NoIcon);
            blockDialog.addButton(TRANS("ok"), 1);
            blockDialog.runModalLoop();
            m_track.loadFileIntoTransport(file);
            break;
        }
        case 4:
        {
            int alertResult = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                TRANS("Confirm"),
                TRANS("Do you really want to remove the audio file from this track?"),
                TRANS("Yes"),
                TRANS("No"),
                this,
                nullptr);

            if (alertResult == 1)
                m_track.unloadFile();

            break;
        }
        case 5:
            m_removeTrackCallback(&m_track);
        }
    }
}

void TrackUi::sliderValueChanged(Slider* /*slider*/)
{
    m_track.setGain(static_cast<float>(m_volumeSlider.getValue()));
}

void TrackUi::updateIdText()
{
    std::stringstream stream;
    stream << m_track.getTrackIndex();
    stream << " ";
    stream << (m_track.isStereo() ? TRANS("St") : TRANS("Mo"));
    m_idLabel.setText(stream.str(), sendNotification);
}

void TrackUi::fileChanged(const File& file, bool updatePlaylist)
{
    m_fileNameLabel.setText(file.getFileName(), sendNotification);
    int textWidth = m_fileNameLabel.getFont().getStringWidth(m_fileNameLabel.getText())
        + m_fileNameLabel.getBorderSize().getLeft() + m_fileNameLabel.getBorderSize().getRight();
    m_fileNameLabel.setBounds(getWidth() - textWidth, getHeight() - 20, textWidth, 20);

    if (updatePlaylist)
    {
        m_fileLoadedCallback(file.getFileNameWithoutExtension());
    }
}

void TrackUi::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    repaint();
}

void TrackUi::mouseDown(const MouseEvent& event)
{
    mouseDrag(event);
}

void TrackUi::mouseDrag(const MouseEvent& event)
{
    if (event.eventComponent != this)
        return;

    const static int componentWidth = 100 + 40 + 20;
    const int xPosition = event.x - componentWidth;

    if (xPosition < 0)
        return;

    const double positionFraction = static_cast<double>(xPosition) / static_cast<double>(getWidth() - componentWidth);
    m_setPositionCallback(positionFraction * m_longestDuration);
}

void TrackUi::setLongestDuration(double duration)
{
    m_longestDuration = duration;
    repaint();
}

void TrackUi::positionChanged(double position)
{
    const static int componentWidth = 100 + 40 + 20;
    const int drawWidth = getWidth() - componentWidth;

    int old_lineX = componentWidth + static_cast<int>(drawWidth * (std::isnan(m_progress) ? 0 : m_progress));
    m_progress = position / m_longestDuration;
    int new_lineX = componentWidth + static_cast<int>(drawWidth * (std::isnan(m_progress) ? 0 : m_progress));

    repaint(old_lineX - 0, 0, new_lineX + 0, getHeight());
}

void TrackUi::loadFile()
{
    FileChooser myChooser(TRANS("Please select the audio file you want to load ..."),
        File(),
        m_track.getAudioFormatManager().getWildcardForAllFormats());
    if (!myChooser.browseForFileToOpen())
        return;

    loadFile(File(myChooser.getResult()));
}

void TrackUi::loadFile(const File& audioFile)
{
    std::unique_ptr<AudioFormatReader> reader{m_track.getAudioFormatManager().createReaderFor(audioFile)};

    if (reader != nullptr)
    {
        if (reader->numChannels > 2)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                TRANS("MStarPlayer"),
                TRANS("The selected file has more than two channels. This is not supported."));
            return;
        }

        if (!m_track.isStereo() && reader->numChannels != 1)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                TRANS("MStarPlayer"),
                String::formatted(
                    TRANS("The selected file has %d channels but this is a mono track."), reader->numChannels));
            return;
        }

        if (m_track.isStereo() && reader->numChannels != 2)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                "MStarPlayer",
                String::formatted(
                    TRANS("The selected file has %d channel(s) but this is a stereo track."), reader->numChannels));
            return;
        }
    }

    m_track.loadFileIntoTransport(audioFile);

    repaint();
}

void TrackUi::paint(Graphics& g)
{
    if (m_track.getTrackIndex() > 1)
        g.drawVerticalLine(0, 0.0f, static_cast<float>(getWidth()));

    const static int componentWidth = 100 + 40 + 20;
    int drawWidth = getWidth() - componentWidth;
    if (m_longestDuration != 0)
        drawWidth = static_cast<int>(drawWidth * m_track.getAudioThumbnail().getTotalLength() / m_longestDuration);
    m_waveform.setAudioThumbnail(&m_track.getAudioThumbnail());
}

void TrackUi::paintOverChildren(Graphics& g)
{
    const static int componentWidth = 100 + 40 + 20;
    int drawWidth = getWidth() - componentWidth;
    const int lineX = componentWidth + static_cast<int>(drawWidth * (std::isnan(m_progress) ? 0 : m_progress));

    g.setColour(Colour(255, 0, 0));
    g.drawVerticalLine(lineX, 0.0f, static_cast<float>(getHeight()));
}

void TrackUi::resized()
{
    m_idLabel.setBounds(0, 0, 100, 20);
    m_descriptionLabel.setBounds(0, 20, 100, getHeight() - 20);

    static const int buttonWidth = 40;
    const static int componentWidth = 100 + 40 + 20;
    int drawWidth = getWidth() - componentWidth;

    m_volumeSlider.setBounds(100 + 3, 3, 20, getHeight() - 6);

    m_editButton.setBounds(100 + 20 + 3, 3, buttonWidth - 6, getHeight() / 3 - 6);
    m_soloButton.setBounds(100 + 20 + 3, 3 + getHeight() / 3, buttonWidth - 6, getHeight() / 3 - 6);
    m_muteButton.setBounds(100 + 20 + 3, 3 + getHeight() * 2 / 3, buttonWidth - 6, getHeight() / 3 - 6);

    int textWidth = 0;
    if (m_fileNameLabel.getText() != "")
        textWidth = m_fileNameLabel.getFont().getStringWidth(m_fileNameLabel.getText())
            + m_fileNameLabel.getBorderSize().getLeft() + m_fileNameLabel.getBorderSize().getRight();
    m_fileNameLabel.setBounds(getWidth() - textWidth, getHeight() - 20, textWidth, 20);

    m_waveform.setBounds(componentWidth, 0, drawWidth, getHeight());
}
