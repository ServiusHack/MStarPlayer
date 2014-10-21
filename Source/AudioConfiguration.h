/*
  ==============================================================================

    AudioConfiguration.h
    Created: 13 Sep 2013 2:37:51pm
    Author:  Severin Leonhardt

  ==============================================================================
*/

#ifndef AUDIOCONFIGURATION_H_INCLUDED
#define AUDIOCONFIGURATION_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
  Simple component showing the JUCE AudioDeviceSelectorComponent.
*/
class AudioConfigurationComponent    : public Component
{
public:
    AudioConfigurationComponent(AudioDeviceManager& audioDeviceManager);

    void resized();

private:
    /**
        JUCE component to configure the audio device for audio output.
    */
    ScopedPointer<AudioDeviceSelectorComponent> selector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioConfigurationComponent)
};

//==============================================================================
/**
    Window wrapper for the AudioConfigurationComponent.

    This allows the component to be shown in its own window.
*/
class AudioConfigurationWindow    : public DialogWindow
{
public:
    AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager);
    
    /** Delete (and thus close) the window when requested by the user.
    */
    void closeButtonPressed();

private:
    ScopedPointer<AudioConfigurationComponent> component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioConfigurationWindow)
};


#endif  // AUDIOCONFIGURATION_H_INCLUDED
