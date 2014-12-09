#include "TrackUi.h"

#include <sstream>

TrackUi::TrackUi(Track& track, ApplicationProperties& applicationProperties, SetPositionCallback setPositionCallback)
	: m_track(track)
	, m_longestDuration(0)
	, m_progress(0)
	, m_applicationProperties(applicationProperties)
	, m_setPositionCallback(setPositionCallback)
{
	m_idLabel = new Label();
	addAndMakeVisible(m_idLabel);
	updateIdText();

	m_descriptionLabel = new Label();
	addAndMakeVisible(m_descriptionLabel);
	m_descriptionLabel->setText(getName(), sendNotification);
	m_descriptionLabel->setJustificationType(Justification::topLeft);

	m_volumeSlider = new VolumeSlider();
	addAndMakeVisible(m_volumeSlider);
	m_volumeSlider->setValue(1.0);
	m_volumeSlider->addListener(this);

	m_editButton = new ImageButton("edit");
	Image editImage = ImageFileFormat::loadFrom(BinaryData::configure_png, BinaryData::configure_pngSize);
	m_editButton->setImages(true, true, true,
		editImage, 0.7f, Colours::transparentBlack,
		editImage, 1.0f, Colours::transparentBlack,
		editImage, 1.0f, Colours::transparentBlack,
		0.0f);
	addAndMakeVisible(m_editButton);
	m_editButton->addListener(this);

	m_soloButton = new ImageButton("solo");
	Image soloImage = ImageFileFormat::loadFrom(BinaryData::audioheadphones_png, BinaryData::audioheadphones_pngSize);
	m_soloButton->setClickingTogglesState(true);
	m_soloButton->addListener(this);
	m_soloButton->setImages(true, true, true,
		soloImage, 0.7f, Colours::transparentBlack,
		soloImage, 1.0f, Colours::transparentBlack,
		soloImage, 1.0f, Colours::red.withAlpha(0.5f),
		0.0f);
	addAndMakeVisible(m_soloButton);

	m_muteButton = new ImageButton("mute");
	Image muteedImage = ImageFileFormat::loadFrom(BinaryData::audiovolumemuted_png, BinaryData::audiovolumemuted_pngSize);
	Image unmutedImage = ImageFileFormat::loadFrom(BinaryData::audiovolumemedium_png, BinaryData::audiovolumemedium_pngSize);
	m_muteButton->setImages(true, true, true,
		unmutedImage, 0.7f, Colours::transparentBlack,
		unmutedImage, 1.0f, Colours::transparentBlack,
		muteedImage, 1.0f, Colours::transparentBlack,
		0.0f);
	m_muteButton->setClickingTogglesState(true);
	m_muteButton->addListener(this);
	addAndMakeVisible(m_muteButton);

	m_fileNameLabel = new Label("filename label");
	m_fileNameLabel->setColour(Label::backgroundColourId, Colour(0xccffffff));
	addAndMakeVisible(m_fileNameLabel);

	m_track.addChangeListener(this);
	m_track.getAudioThumbnail().addChangeListener(this);
	m_track.addPositionCallback(std::bind(&TrackUi::positionChanged, this, std::placeholders::_1));
	m_track.setFileChangedCallback(std::bind(&Label::setText, m_fileNameLabel.get(), std::placeholders::_1, sendNotification));
}

TrackUi::~TrackUi()
{
	m_volumeSlider->removeListener(this);
}

void TrackUi::gainChanged(float gain)
{
	m_volumeSlider->setValue(gain);
}

void TrackUi::muteChanged(bool mute)
{
	m_muteButton->setToggleState(mute, sendNotification);
}

void TrackUi::soloChanged(bool solo)
{
	m_soloButton->setToggleState(solo, sendNotification);
}

void TrackUi::nameChanged(const String& name)
{
	m_descriptionLabel->setText(name, sendNotification);
}

void TrackUi::buttonClicked(Button *button)
{
	if (button == m_muteButton) {
		m_track.setMute(m_muteButton->getToggleState());
	}
	else if (button == m_soloButton) {
		m_track.setSolo(m_soloButton->getToggleState());
	}
	else if (button == m_editButton) {
		PopupMenu m;
		m.addItem (1, "edit track");
		m.addItem (2, "open file");
		m.addItem (3, "edit file");
		m.addItem (4, "remove file");
		m.addSeparator();
		const int result = m.show();

		switch (result)
		{
		case 1:
			m_editDialog = ScopedPointer<TrackEditDialogWindow>(new TrackEditDialogWindow(m_track.getName(), m_track.getGain(),
				std::bind(&Track::setName, &m_track, std::placeholders::_1),
				std::bind(&Track::setGain, &m_track, std::placeholders::_1)));
			break;
		case 2:
			loadFile();
			break;
		case 3:
		{
			if (!File(m_applicationProperties.getUserSettings()->getValue("audioEditor")).existsAsFile()) {
				AlertWindow::showMessageBox(AlertWindow::WarningIcon, TRANS("No audio editor"), TRANS("No audio editor was configured."));
				return;
			}

			ChildProcess process;
			StringArray arguments(m_applicationProperties.getUserSettings()->getValue("audioEditor"));
			arguments.add(m_track.getTrackConfig().file.getFullPathName());
			if (!process.start(arguments)) {
				AlertWindow::showMessageBox(AlertWindow::WarningIcon, TRANS("Failed launch audio editor"), TRANS("Failed to start the audio editor."));
				return;
			}

			AlertWindow blockDialog("Audio editor launched", "Modify the file in the audio editor. Click on 'ok' after the file was saved to load it again.", AlertWindow::NoIcon);
			blockDialog.addButton("ok", 1);
			blockDialog.runModalLoop();
			m_track.reloadFile();
			break;
		}
		case 4:
		{
			int result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
				TRANS("Confirm"),
				TRANS("Do you really want to remove the audio file from this track?"),
				TRANS("Yes"),
				TRANS("No"),
				this,
				nullptr);

			if (result == 1)
				m_track.unloadFile();
		}
		}
	}
}

void TrackUi::sliderValueChanged(Slider* /*slider*/)
{
	m_track.setGain(static_cast<float>(m_volumeSlider->getValue()));
}

void TrackUi::updateIdText()
{
	std::stringstream stream;
	stream << m_track.getTrackIndex();
	stream << " ";
	stream << (m_track.isStereo() ? "St" : "Mo");
	m_idLabel->setText(stream.str(), sendNotification);
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
	int xPosition = event.x - componentWidth;

	if (xPosition < 0)
		return;

	double positionFraction = static_cast<double>(xPosition) / static_cast<double>(getWidth() - componentWidth);
	m_setPositionCallback(positionFraction * m_longestDuration);
}

void TrackUi::setLongestDuration(double duration)
{
	m_longestDuration = duration;
	repaint();
}

void TrackUi::positionChanged(double position)
{
	m_progress = position / m_longestDuration;
	repaint();
}

void TrackUi::loadFile()
{
	FileChooser myChooser ("Please select the audio file you want to load ...",
			File::getSpecialLocation (File::userHomeDirectory),
			m_track.getAudioFormatManager().getWildcardForAllFormats());
	if (!myChooser.browseForFileToOpen())
		return;

	File audioFile = File(myChooser.getResult());

	AudioFormatReader* reader = m_track.getAudioFormatManager().createReaderFor(audioFile);

	if (reader != nullptr) {
		if (reader->numChannels > 2) {
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
				"AudioPlayerJuce",
				"The selected file has more than two channels. This is not supported."
				);
			return;
		}

		if (!m_track.isStereo() && reader->numChannels != 1) {
			int result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
				"AudioPlayerJuce",
				"You have selected a stereo file for a mono track.",
				"Convert to stereo track",
				"Abort",
				this,
				nullptr
				);
			switch (result) {
			case 1:
				m_track.setStereo(true);
				updateIdText();
				break;
			default:
				return;
			}
		}

		if (m_track.isStereo() && reader->numChannels != 2) {
			int result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
				"AudioPlayerJuce",
				"You have selected a mono file for a stereo track.",
				"Convert to mono track",
				"Abort",
				this,
				nullptr
				);
			switch (result) {
			case 1:
				m_track.setStereo(false);
				updateIdText();
				break;
			default:
				return;
			}
		}

		delete reader;
	}

	m_track.loadFileIntoTransport(audioFile);

	m_fileNameLabel->setText(audioFile.getFileName(), sendNotification);
	int textWidth = m_fileNameLabel->getFont().getStringWidth(m_fileNameLabel->getText()) + m_fileNameLabel->getBorderSize().getLeft() + m_fileNameLabel->getBorderSize().getRight();
	m_fileNameLabel->setBounds(getWidth() - textWidth, getHeight() - 20, textWidth, 20);
	repaint();
}

void TrackUi::paint (Graphics& g)
{
	if (m_track.getTrackIndex() > 1)
		g.drawLine(0.0f, 0.0f, static_cast<float>(getWidth()), 0.0f);

	const static int componentWidth = 100 + 40 + 20;
	int drawWidth = getWidth() - componentWidth;
	if (m_longestDuration != 0)
		drawWidth = static_cast<int>(drawWidth * m_track.getAudioThumbnail().getTotalLength() / m_longestDuration);
	m_track.getAudioThumbnail().drawChannels(g, Rectangle<int>(componentWidth, 0, drawWidth, getHeight()), 0, m_track.getAudioThumbnail().getTotalLength(), 1.0f);

	g.setColour(Colour(255, 0, 0));
	drawWidth = getWidth() - componentWidth;

	float lineX = componentWidth + static_cast<float>(drawWidth * (std::isnan(m_progress) ? 0 : m_progress));
	g.drawLine(lineX, 0.0f, lineX, static_cast<float>(getHeight()));
}

void TrackUi::resized()
{
	m_idLabel->setBounds(0, 0, 100, 20);
	m_descriptionLabel->setBounds(0, 20, 100, getHeight() - 20);

	static const int buttonWidth = 40;

	m_volumeSlider->setBounds(100 + 3, 3, 20, getHeight() - 6);
	
	m_editButton->setBounds(100 + 20 + 3, 3, buttonWidth - 6, getHeight() / 3 - 6);
	m_soloButton->setBounds(100 + 20 + 3, 3 + getHeight() / 3, buttonWidth - 6, getHeight() / 3 - 6);
	m_muteButton->setBounds(100 + 20 + 3, 3 + getHeight() * 2 / 3, buttonWidth - 6, getHeight() / 3 - 6);

	int textWidth = 0;
	if (m_fileNameLabel->getText() != "")
		textWidth = m_fileNameLabel->getFont().getStringWidth(m_fileNameLabel->getText()) + m_fileNameLabel->getBorderSize().getLeft() + m_fileNameLabel->getBorderSize().getRight();
	m_fileNameLabel->setBounds(getWidth() - textWidth, getHeight() - 20, textWidth, 20);
}
