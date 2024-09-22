#include "AudioDeviceComponent.h"

AudioDeviceComponent::AudioDeviceComponent(juce::AudioDeviceManager& audioDeviceManager)
    : selector(audioDeviceManager, 0, 0, 1, 64, false, false, false, false)
    , selectAllChannelsButton("selectAll")
    , deselectAllChannelsButton("deselectAll")
{
    addAndMakeVisible(selectAllChannelsButton);
    selectAllChannelsButton.setButtonText(TRANS("activate all channels"));
    selectAllChannelsButton.addListener(this);
    selectAllChannelsButton.setWantsKeyboardFocus(false);

    addAndMakeVisible(deselectAllChannelsButton);
    deselectAllChannelsButton.setButtonText(TRANS("deactivate all but first channel"));
    deselectAllChannelsButton.addListener(this);
    deselectAllChannelsButton.setWantsKeyboardFocus(false);

    addAndMakeVisible(selector);
}

void AudioDeviceComponent::resized()
{
    const static int buttonHeight = 24;
    const static int padding = 10;

    const int buttonWidth = (getWidth() - 3 * padding) / 2;

    selectAllChannelsButton.setBounds(padding, padding, buttonWidth, buttonHeight);
    deselectAllChannelsButton.setBounds(2 * padding + buttonWidth, padding, buttonWidth, buttonHeight);

    selector.setBounds(0, 2 * padding + buttonHeight, getWidth(), selector.getHeight());
}

void AudioDeviceComponent::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &selectAllChannelsButton)
    {
        juce::AudioDeviceManager& manager = selector.deviceManager;
        juce::AudioDeviceManager::AudioDeviceSetup setup = manager.getAudioDeviceSetup();
        setup.useDefaultOutputChannels = false;
        for (int i = 0; i < manager.getCurrentAudioDevice()->getOutputChannelNames().size(); ++i)
        {
            setup.outputChannels.setBit(i);
        }
        manager.setAudioDeviceSetup(setup, true);
    }
    else if (buttonThatWasClicked == &deselectAllChannelsButton)
    {
        juce::AudioDeviceManager& manager = selector.deviceManager;
        juce::AudioDeviceManager::AudioDeviceSetup setup = manager.getAudioDeviceSetup();
        setup.useDefaultOutputChannels = false;
        for (int i = 1; i < manager.getCurrentAudioDevice()->getOutputChannelNames().size(); ++i)
        {
            setup.outputChannels.clearBit(i);
        }
        manager.setAudioDeviceSetup(setup, true);
    }
}
