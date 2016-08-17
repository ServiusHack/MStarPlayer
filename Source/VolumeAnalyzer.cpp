#include "VolumeAnalyzer.h"

VolumeAnalyzer::VolumeAnalyzer(size_t bufferSize)
    : m_samples(bufferSize, 0.0f)
    , m_writeIndex(0)
{
    jassert(bufferSize > 0);
}

float VolumeAnalyzer::getVolume() const
{
    return *std::max_element(m_samples.begin(), m_samples.end());
}

void VolumeAnalyzer::update(const float* buffer, int numSamples)
{
    for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
        m_samples[++m_writeIndex % m_samples.size()] = std::abs(buffer[sampleIndex]);
}
