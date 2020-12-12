#include "VolumeAnalyzer.h"

#include <cmath>

VolumeAnalyzer::VolumeAnalyzer(float decayRate)
    : m_decayRate(decayRate)
{
}

float VolumeAnalyzer::getVolume() const
{
    return m_volume;
}

void VolumeAnalyzer::update(const float* buffer, int numSamples)
{
    for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
    {
        float abs = std::abs(buffer[sampleIndex]);
        if (abs >= m_volume)
            m_volume = abs;
        else
            m_volume *= (1.0f - 1.0f / m_decayRate);
    }
}
