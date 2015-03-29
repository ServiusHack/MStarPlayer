#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
	Keep track of a fixed number of samples to determine their loudness.
*/
class VolumeAnalyzer
{
public:
	VolumeAnalyzer(size_t bufferSize);

	float getVolume() const;
	void update(const float* buffer, int numSamples);

private:
	std::vector<float> m_samples;
	size_t m_writeIndex;
};
