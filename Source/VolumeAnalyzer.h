#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
        Keep track of a fixed number of samples to determine their loudness.
*/
class VolumeAnalyzer
{
public:
    VolumeAnalyzer(float decayRate);

    float getVolume() const;
    void update(const float* buffer, int numSamples);

private:
    float m_decayRate;
    float m_volume{};
};
