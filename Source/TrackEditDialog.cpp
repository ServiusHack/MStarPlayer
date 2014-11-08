#include "TrackEditDialog.h"


TrackEditDialogWindow::TrackEditDialogWindow(String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback)
	: DialogWindow("Edit track", Colours::lightgrey, true, true)
{
	TrackEditDialogComponent* component = new TrackEditDialogComponent(name, trackGain, settingsChangedCallback, volumeChangedCallback, this);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(false, false);
}

void TrackEditDialogWindow::closeButtonPressed()
{
	setVisible(false);
}

bool TrackEditDialogWindow::keyPressed(const KeyPress &key)
{
	if (key == KeyPress::returnKey) {
		exitModalState(0);
		return true;
	}

	return false;
}

void TrackEditDialogWindow::focusGained(FocusChangeType /*cause*/)
{
	static_cast<TrackEditDialogComponent*>(getContentComponent())->m_nameEditor->grabKeyboardFocus();
}

TrackEditDialogComponent::TrackEditDialogComponent(String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback, TrackEditDialogWindow* parent)
	: m_parent(parent)
	, m_settingsChangedCallback(settingsChangedCallback)
	, m_volumeChangedCallback(volumeChangedCallback)
{
	addAndMakeVisible(m_nameLabel = new Label("name label",
                                          TRANS("Name of the track:")));
	m_nameLabel->setFont(Font(15.00f, Font::plain));
	m_nameLabel->setJustificationType(Justification::centredLeft);
	m_nameLabel->setEditable(false, false, false);
	m_nameLabel->setColour(TextEditor::textColourId, Colours::black);
	m_nameLabel->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	addAndMakeVisible(m_nameEditor = new TextEditor("name editor"));
	m_nameEditor->setMultiLine(false);
	m_nameEditor->setReturnKeyStartsNewLine(false);
	m_nameEditor->setReadOnly(false);
	m_nameEditor->setScrollbarsShown(true);
	m_nameEditor->setCaretVisible(true);
	m_nameEditor->setPopupMenuEnabled(true);
	m_nameEditor->setEscapeAndReturnKeysConsumed(false);
	m_nameEditor->setText(name);
	m_nameEditor->selectAll();
	m_nameEditor->addListener(this);

	addAndMakeVisible(m_volumeSlider = new Slider("volume slider"));
	m_volumeSlider->setRange(0, 2, 0.1);
	m_volumeSlider->setValue(trackGain);
	m_volumeSlider->setSliderStyle(Slider::LinearVertical);
	m_volumeSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	m_volumeSlider->addListener(this);

	addAndMakeVisible(m_closeButton = new TextButton("close"));
	m_closeButton->setButtonText(TRANS("Close"));
	m_closeButton->addListener(this);
	m_closeButton->setWantsKeyboardFocus(false);

	setWantsKeyboardFocus(false);

    setSize(250, 200);
}

TrackEditDialogComponent::~TrackEditDialogComponent()
{
	m_nameLabel = nullptr;
	m_nameEditor = nullptr;
	m_closeButton = nullptr;
}

void TrackEditDialogComponent::resized()
{
	const static int buttonWidth = 80;
	const static int buttonHeight = 24;
	const static int padding = 10;

	m_volumeSlider->setBounds(padding, padding, 16, getHeight() - buttonHeight - 3 * padding);

	m_nameLabel->setBounds(30, 10, 100, 24);
	m_nameEditor->setBounds(140, 10, 70, 24);

	m_closeButton->setBounds(
		(getWidth() - buttonWidth) / 2,
		getHeight() - buttonHeight - padding,
		buttonWidth,
		buttonHeight
		);
}

void TrackEditDialogComponent::buttonClicked(Button* /*buttonThatWasClicked*/)
{
	m_parent->setVisible(false);
}

void TrackEditDialogComponent::sliderValueChanged(Slider* /*slider*/)
{
	m_volumeChangedCallback(m_volumeSlider->getValue());
}

void TrackEditDialogComponent::textEditorTextChanged(TextEditor &)
{
	m_settingsChangedCallback(m_nameEditor->getText());
}