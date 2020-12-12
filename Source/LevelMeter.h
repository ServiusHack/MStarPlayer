#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

/**
        Draw the volume as a level meter.
*/
class LevelMeter : public juce::Component
{
public:
    static const int refreshRate = 20; // Updates per second.

    LevelMeter();

    void setVolume(float volume);

    virtual void paint(juce::Graphics& g) override;

private:
    const juce::AffineTransform m_rotationTransformation;
    float m_volume;
    float m_peak;
    int m_peakDurationLeft;
};
