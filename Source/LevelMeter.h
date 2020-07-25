#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
        Draw the volume as a level meter.
*/
class LevelMeter : public Component
{
public:
    static const int refreshRate = 20; // Updates per second.

    LevelMeter();

    void setVolume(float volume);

    virtual void paint(Graphics& g) override;

private:
    const AffineTransform m_rotationTransformation;
    float m_volume;
    float m_peak;
    int m_peakDurationLeft;
};
