#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
    Window wrapper for the AudioDeviceSelectorComponent.

    This allows the component to be shown in its own window.
*/
class AudioConfigurationWindow : public DialogWindow
{
public:
    AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager);
    
    /** Delete (and thus close) the window when requested by the user.
    */
    virtual void closeButtonPressed() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioConfigurationWindow)
};
