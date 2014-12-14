#include "../JuceLibraryCode/JuceHeader.h"

#include "EditSettingsDialog.h"


EditSettingsWindow::EditSettingsWindow(ApplicationProperties& applicationProperties)
	: DialogWindow ("Edit Settings", Colours::lightgrey, true, true)
{
	setContentOwned(new EditSettingsComponent(this, applicationProperties), true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(true, true);
}

void EditSettingsWindow::closeButtonPressed()
{
	setVisible(false);
}

void EditSettingsWindow::buttonClicked(Button*)
{
	closeButtonPressed();
}

EditSettingsComponent::EditSettingsComponent(EditSettingsWindow* parent, ApplicationProperties& applicationProperties)
	: m_applicationProperties(applicationProperties)
{
	addAndMakeVisible(m_nameLabel = new Label("audio editor label",
                                          TRANS("Audio editor")));
	m_nameLabel->setFont(Font(15.00f, Font::plain));
	m_nameLabel->setJustificationType(Justification::centredLeft);
	m_nameLabel->setEditable(false, false, false);
	m_nameLabel->setColour(TextEditor::textColourId, Colours::black);
	m_nameLabel->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	addAndMakeVisible(m_audioEditorFilenameComponent = new FilenameComponent("audio editor", File(m_applicationProperties.getUserSettings()->getValue("audioEditor")), true, false, false, "*.exe", ".exe", ""));
	m_audioEditorFilenameComponent->setDefaultBrowseTarget(File::getSpecialLocation(File::globalApplicationsDirectory));
	m_audioEditorFilenameComponent->addListener(this);

	addAndMakeVisible(m_closeButton = new TextButton("close"));
	m_closeButton->setButtonText(TRANS("Close"));
	m_closeButton->addListener(parent);
	m_closeButton->setWantsKeyboardFocus(false);

	setSize(500, 150);
}

void EditSettingsComponent::resized()
{
	const static int buttonWidth = 80;
	const static int rowHeight = 24;
	const static int padding = 10;

	m_nameLabel->setBounds(padding, padding, getWidth() - 2 * padding, rowHeight);
	m_audioEditorFilenameComponent->setBounds(padding, padding + rowHeight, getWidth() - 2 * padding, rowHeight);

	m_closeButton->setBounds(
		(getWidth() - buttonWidth) / 2,
		getHeight() - 2*(rowHeight - padding),
		buttonWidth,
		rowHeight
		);

}

void EditSettingsComponent::filenameComponentChanged(FilenameComponent* /*fileComponentThatHasChanged*/)
{
	m_applicationProperties.getUserSettings()->setValue("audioEditor", m_audioEditorFilenameComponent->getCurrentFileText());
}
