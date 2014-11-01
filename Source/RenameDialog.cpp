/*
==============================================================================

RenameDialog.cpp
Created: 13 Sep 2013 2:37:51pm
Author:  Severin Leonhardt

==============================================================================
*/

#include "RenameDialog.h"


RenameDialogWindow::RenameDialogWindow(String playerName)
: DialogWindow("Rename player", Colours::lightgrey, true, true)
{
	RenameDialogComponent* component = new RenameDialogComponent(playerName, this);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	//setVisible(true);
	//component->textEditor->take
	setResizable(false, false);
}

void RenameDialogWindow::closeButtonPressed()
{
	exitModalState(1);
}

String RenameDialogWindow::getPlayerName()
{
	return static_cast<RenameDialogComponent*>(getContentComponent())->textEditor->getText();
}

bool RenameDialogWindow::keyPressed(const KeyPress &key)
{
	if (key == KeyPress::returnKey) {
		exitModalState(0);
		return true;
	}

	return false;
}

void RenameDialogWindow::focusGained(FocusChangeType cause)
{
	static_cast<RenameDialogComponent*>(getContentComponent())->textEditor->grabKeyboardFocus();
}

RenameDialogComponent::RenameDialogComponent(String playerName, RenameDialogWindow* parent)
	: parent(parent)
{
    addAndMakeVisible (label = new Label ("new label",
                                          TRANS("Name of the player:")));
    label->setFont (Font (15.00f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (textEditor = new TextEditor ("new text editor"));
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
	textEditor->setEscapeAndReturnKeysConsumed(false);
	textEditor->setText(playerName);
	textEditor->selectAll();

    addAndMakeVisible (saveButton = new TextButton ("save"));
	saveButton->setButtonText(TRANS("Save"));
	saveButton->addListener(this);
	saveButton->setWantsKeyboardFocus(false);

	addAndMakeVisible(abortButton = new TextButton("abort"));
	abortButton->setButtonText(TRANS("Abort"));
	abortButton->addListener(this);
	saveButton->setWantsKeyboardFocus(false);

	setWantsKeyboardFocus(false);

    setSize (200, 100);
}

RenameDialogComponent::~RenameDialogComponent()
{
	label = nullptr;
    textEditor = nullptr;
	saveButton = nullptr;
	abortButton = nullptr;
}

void RenameDialogComponent::paint(Graphics& g)
{
    g.fillAll (Colours::white);
}

void RenameDialogComponent::resized()
{
    label->setBounds (0, 0, 192, 24);
    textEditor->setBounds (8, 24, 184, 24);
	saveButton->setBounds(8, 56, 80, 24);
	abortButton->setBounds(112, 56, 80, 24);
}

void RenameDialogComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == saveButton)
	{
		parent->exitModalState(0);
    }
	else if (buttonThatWasClicked == abortButton)
    {
		parent->exitModalState(1);
    }
}