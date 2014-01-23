/*
  ==============================================================================

	AudioConfiguration.cpp
	Created: 13 Sep 2013 2:37:51pm
	Author:  Severin Leonhardt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConfiguration.h"

//==============================================================================
AudioConfigurationWindow::AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager)  : DialogWindow ("Configure Audio",
										Colours::lightgrey,
										true,
										true)
{
	component = new AudioConfigurationComponent(audioDeviceManager);
	component->setSize(400,400);
	
	setContentOwned (component, true);
	centreWithSize (getWidth(), getHeight());
	setVisible (true);
	setResizable(true, true);
}

void AudioConfigurationWindow::closeButtonPressed()
{
	delete this;
}

//==============================================================================
AudioConfigurationComponent::AudioConfigurationComponent(AudioDeviceManager& audioDeviceManager)
{
	selector = new AudioDeviceSelectorComponent(audioDeviceManager,0, 0, 1, 64, false, false, false, false);
	addAndMakeVisible(selector);
}

void AudioConfigurationComponent::resized()
{
	selector->setBounds(this->getBounds());
}
