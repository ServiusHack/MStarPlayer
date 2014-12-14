#include "PlaylistEntryDialog.h"

PlaylistEntryDialogWindow::PlaylistEntryDialogWindow(String name, PlaylistEntrySettingsChangedCallback changedCallback)
	: DialogWindow("Edit playlist entry", Colours::lightgrey, true, true)
{
	PlaylistEntryDialogComponent* component = new PlaylistEntryDialogComponent(name, changedCallback, this);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(false, false);
}

void PlaylistEntryDialogWindow::closeButtonPressed()
{
	setVisible(false);
}

bool PlaylistEntryDialogWindow::keyPressed(const KeyPress &key)
{
	if (key == KeyPress::returnKey) {
		exitModalState(0);
		return true;
	}

	return false;
}

void PlaylistEntryDialogWindow::focusGained(FocusChangeType /*cause*/)
{
	static_cast<PlaylistEntryDialogComponent*>(getContentComponent())->m_nameEditor->grabKeyboardFocus();
}

void PlaylistEntryDialogWindow::buttonClicked(Button* /*buttonThatWasClicked*/)
{
	closeButtonPressed();
}

PlaylistEntryDialogComponent::PlaylistEntryDialogComponent(const String& name, const PlaylistEntrySettingsChangedCallback& changedCallback, PlaylistEntryDialogWindow* parent)
    : m_changedCallback(changedCallback)
{
	addAndMakeVisible(m_nameLabel = new Label("name label",
                                          TRANS("Name of the entry:")));
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

	addAndMakeVisible(m_closeButton = new TextButton("close"));
	m_closeButton->setButtonText(TRANS("Close"));
	m_closeButton->addListener(parent);
	m_closeButton->setWantsKeyboardFocus(false);

	setWantsKeyboardFocus(false);

    setSize(250, 200);
}

PlaylistEntryDialogComponent::~PlaylistEntryDialogComponent()
{
	m_nameLabel = nullptr;
	m_nameEditor = nullptr;
	m_closeButton = nullptr;
}

void PlaylistEntryDialogComponent::resized()
{
	const static int buttonWidth = 80;
	const static int rowHeight = 24;
	const static int padding = 10;

	m_nameLabel->setBounds(padding, padding, getWidth() - 2 * padding, rowHeight);
	m_nameEditor->setBounds(padding, padding + rowHeight, getWidth() - 2 * padding, rowHeight);

	m_closeButton->setBounds(
		(getWidth() - buttonWidth) / 2,
		getHeight() - 2*(rowHeight - padding),
		buttonWidth,
		rowHeight
		);
}

void PlaylistEntryDialogComponent::textEditorTextChanged(TextEditor &)
{
	m_changedCallback(m_nameEditor->getText());
}