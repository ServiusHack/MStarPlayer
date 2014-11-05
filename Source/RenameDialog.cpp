#include "RenameDialog.h"

RenameDialogWindow::RenameDialogWindow(String playerName)
	: DialogWindow("Rename player", Colours::lightgrey, true, true)
{
	RenameDialogComponent* component = new RenameDialogComponent(playerName, this);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setResizable(false, false);
}

void RenameDialogWindow::closeButtonPressed()
{
	exitModalState(1);
}

String RenameDialogWindow::getPlayerName()
{
	return static_cast<RenameDialogComponent*>(getContentComponent())->m_textEditor->getText();
}

bool RenameDialogWindow::keyPressed(const KeyPress &key)
{
	if (key == KeyPress::returnKey) {
		exitModalState(0);
		return true;
	}

	return false;
}

void RenameDialogWindow::focusGained(FocusChangeType /*cause*/)
{
	static_cast<RenameDialogComponent*>(getContentComponent())->m_textEditor->grabKeyboardFocus();
}

RenameDialogComponent::RenameDialogComponent(String playerName, RenameDialogWindow* parent)
: m_parent(parent)
{
	addAndMakeVisible(m_label = new Label("new label",
                                          TRANS("Name of the player:")));
	m_label->setFont(Font(15.00f, Font::plain));
	m_label->setJustificationType(Justification::centredLeft);
	m_label->setEditable(false, false, false);
	m_label->setColour(TextEditor::textColourId, Colours::black);
	m_label->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	addAndMakeVisible(m_textEditor = new TextEditor("new text editor"));
	m_textEditor->setMultiLine(false);
	m_textEditor->setReturnKeyStartsNewLine(false);
	m_textEditor->setReadOnly(false);
	m_textEditor->setScrollbarsShown(true);
	m_textEditor->setCaretVisible(true);
	m_textEditor->setPopupMenuEnabled(true);
	m_textEditor->setEscapeAndReturnKeysConsumed(false);
	m_textEditor->setText(playerName);
	m_textEditor->selectAll();

	addAndMakeVisible(m_saveButton = new TextButton("save"));
	m_saveButton->setButtonText(TRANS("Save"));
	m_saveButton->addListener(this);
	m_saveButton->setWantsKeyboardFocus(false);

	addAndMakeVisible(m_abortButton = new TextButton("abort"));
	m_abortButton->setButtonText(TRANS("Abort"));
	m_abortButton->addListener(this);
	m_abortButton->setWantsKeyboardFocus(false);

	setWantsKeyboardFocus(false);

    setSize(200, 100);
}

RenameDialogComponent::~RenameDialogComponent()
{
	m_label = nullptr;
	m_textEditor = nullptr;
	m_saveButton = nullptr;
	m_abortButton = nullptr;
}

void RenameDialogComponent::resized()
{
	m_label->setBounds(0, 0, 192, 24);
	m_textEditor->setBounds(8, 24, 184, 24);
	m_saveButton->setBounds(8, 56, 80, 24);
	m_abortButton->setBounds(112, 56, 80, 24);
}

void RenameDialogComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_saveButton)
		m_parent->exitModalState(0);
	else if (buttonThatWasClicked == m_abortButton)
		m_parent->exitModalState(1);
}