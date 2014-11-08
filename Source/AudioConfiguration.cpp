#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConfiguration.h"

AudioConfigurationWindow::AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager)
	: DialogWindow ("Configure Audio", Colours::lightgrey, true, true)
{
	AudioConfigurationComponent* component = new AudioConfigurationComponent(this, audioDeviceManager);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(true, true);
}

void AudioConfigurationWindow::closeButtonPressed()
{
	delete this;
}

AudioConfigurationComponent::AudioConfigurationComponent(AudioConfigurationWindow* parent, AudioDeviceManager& audioDeviceManager)
	: m_parent(parent)
{
	addAndMakeVisible(m_selector = new AudioDeviceSelectorComponent(audioDeviceManager, 0, 0, 1, 64, false, false, false, false));

	addAndMakeVisible(m_closeButton = new TextButton("close"));
	m_closeButton->setButtonText(TRANS("Close"));
	m_closeButton->addListener(this);
	m_closeButton->setWantsKeyboardFocus(false);

	setSize(700, 400);
}

void AudioConfigurationComponent::resized()
{
	const static int buttonWidth = 80;
	const static int buttonHeight = 24;
	const static int padding = 10;
	m_selector->setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - buttonHeight - 3 * padding);
	m_closeButton->setBounds(
		(getWidth() - buttonWidth) / 2,
		getHeight() - buttonHeight - padding,
		buttonWidth,
		buttonHeight
	);
}

void AudioConfigurationComponent::buttonClicked(Button* /*buttonThatWasClicked*/)
{
	m_parent->setVisible(false);
}