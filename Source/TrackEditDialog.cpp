#include "TrackEditDialog.h"


TrackEditDialogWindow::TrackEditDialogWindow(String name, TrackSettingsChangedCallback settingsChangedCallback)
	: DialogWindow("TrackEdit player", Colours::lightgrey, true, true)
{
	TrackEditDialogComponent* component = new TrackEditDialogComponent(name, settingsChangedCallback, this);
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

TrackEditDialogComponent::TrackEditDialogComponent(String name, TrackSettingsChangedCallback settingsChangedCallback, TrackEditDialogWindow* parent)
	: m_parent(parent)
	, m_settingsChangedCallback(settingsChangedCallback)
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

	addAndMakeVisible(m_saveButton = new TextButton("save"));
	m_saveButton->setButtonText(TRANS("Save"));
	m_saveButton->addListener(this);
	m_saveButton->setWantsKeyboardFocus(false);

	addAndMakeVisible(m_abortButton = new TextButton("abort"));
	m_abortButton->setButtonText(TRANS("Abort"));
	m_abortButton->addListener(this);
	m_abortButton->setWantsKeyboardFocus(false);

	setWantsKeyboardFocus(false);

    setSize (200, 100);
}

TrackEditDialogComponent::~TrackEditDialogComponent()
{
	m_nameLabel = nullptr;
	m_nameEditor = nullptr;
	m_saveButton = nullptr;
	m_abortButton = nullptr;
}

void TrackEditDialogComponent::resized()
{
	m_nameLabel->setBounds(0, 0, 100, 24);
	m_nameEditor->setBounds(110, 0, 50, 24);
	m_saveButton->setBounds(8, 56, 80, 24);
	m_abortButton->setBounds(112, 56, 80, 24);
}

void TrackEditDialogComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_saveButton)
	{
		m_settingsChangedCallback(m_nameEditor->getText());
		m_parent->setVisible(false);
    }
	else if (buttonThatWasClicked == m_abortButton)
    {
		m_parent->setVisible(false);
    }
}