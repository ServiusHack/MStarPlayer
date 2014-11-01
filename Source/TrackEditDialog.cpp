/*
==============================================================================

TrackEditDialog.cpp
Created: 13 Sep 2013 2:37:51pm
Author:  Severin Leonhardt

==============================================================================
*/

#include "TrackEditDialog.h"


TrackEditDialogWindow::TrackEditDialogWindow(String name, TrackSettingsChangedCallback settingsChangedCallback)
: DialogWindow("TrackEdit player", Colours::lightgrey, true, true)
{
	TrackEditDialogComponent* component = new TrackEditDialogComponent(name, settingsChangedCallback, this);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	//component->textEditor->take
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

void TrackEditDialogWindow::focusGained(FocusChangeType cause)
{
	static_cast<TrackEditDialogComponent*>(getContentComponent())->nameEditor->grabKeyboardFocus();
}

TrackEditDialogComponent::TrackEditDialogComponent(String name, TrackSettingsChangedCallback settingsChangedCallback, TrackEditDialogWindow* parent)
	: parent(parent)
	, settingsChangedCallback(settingsChangedCallback)
{
    addAndMakeVisible (nameLabel = new Label ("name label",
                                          TRANS("Name of the track:")));
	nameLabel->setFont(Font(15.00f, Font::plain));
	nameLabel->setJustificationType(Justification::centredLeft);
	nameLabel->setEditable(false, false, false);
	nameLabel->setColour(TextEditor::textColourId, Colours::black);
	nameLabel->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    addAndMakeVisible (nameEditor = new TextEditor ("name editor"));
	nameEditor->setMultiLine(false);
	nameEditor->setReturnKeyStartsNewLine(false);
	nameEditor->setReadOnly(false);
	nameEditor->setScrollbarsShown(true);
	nameEditor->setCaretVisible(true);
	nameEditor->setPopupMenuEnabled(true);
	nameEditor->setEscapeAndReturnKeysConsumed(false);
	nameEditor->setText(name);
	nameEditor->selectAll();

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

TrackEditDialogComponent::~TrackEditDialogComponent()
{
	nameLabel = nullptr;
	nameEditor = nullptr;
	saveButton = nullptr;
	abortButton = nullptr;
}

void TrackEditDialogComponent::paint(Graphics& g)
{
    g.fillAll (Colours::white);
}

void TrackEditDialogComponent::resized()
{
	nameLabel->setBounds(0, 0, 100, 24);
	nameEditor->setBounds(110, 0, 50, 24);
	saveButton->setBounds(8, 56, 80, 24);
	abortButton->setBounds(112, 56, 80, 24);
}

void TrackEditDialogComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == saveButton)
	{
		settingsChangedCallback(nameEditor->getText());
		parent->setVisible(false);
    }
	else if (buttonThatWasClicked == abortButton)
    {
		parent->setVisible(false);
    }
}