#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
        Draw a waveform.
*/
class Waveform
    : public Component
    , public ChangeListener
{
public:
    Waveform(Colour color = Colours::black);

    void setAudioThumbnail(AudioThumbnail* audioThumbnail);

    virtual void paint(Graphics& g) override;

private:
    void changeListenerCallback(ChangeBroadcaster* source) override;
    Colour m_color;
    AudioThumbnail* m_audioThumbnail;
};
