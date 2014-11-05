#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConfiguration.h"

AudioConfigurationWindow::AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager)
	: DialogWindow ("Configure Audio", Colours::lightgrey, true, true)
{
	AudioDeviceSelectorComponent* component = new AudioDeviceSelectorComponent(audioDeviceManager, 0, 0, 1, 64, false, false, false, false);
	component->setSize(400,400);
	
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(true, true);
}

void AudioConfigurationWindow::closeButtonPressed()
{
	delete this;
}