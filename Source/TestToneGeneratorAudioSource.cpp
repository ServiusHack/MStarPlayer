#include "TestToneGeneratorAudioSource.h"

TestToneGeneratorAudioSource::TestToneGeneratorAudioSource()
    : dis(-1.0, 1.0)
    , gen(std::random_device()())
{
}

void TestToneGeneratorAudioSource::setMode(Mode mode)
{
    this->mode = mode;
}

void TestToneGeneratorAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_samplesPerBlockExpected = samplesPerBlockExpected;
    m_sampleRate = sampleRate;

    const double cyclesPerSample = 1000 / m_sampleRate; // [2]
    angleDelta = cyclesPerSample * 2.0 * double_Pi;                                // [3]

    prepared.resize(m_samplesPerBlockExpected);
}

void TestToneGeneratorAudioSource::releaseResources()
{
}

void TestToneGeneratorAudioSource::setNumChannels(int numberOfChannels)
{
    const ScopedLock sl(m_lock);
    channels.resize(numberOfChannels);
}

void TestToneGeneratorAudioSource::setEnable(int channel, bool enable)
{
    const ScopedLock sl(m_lock);
    channels.set(channel, enable);
}

void TestToneGeneratorAudioSource::setVolume(double volume)
{
    level = volume;
}

double TestToneGeneratorAudioSource::getVolume() const
{
    return level;
}

void TestToneGeneratorAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    const ScopedLock sl(m_lock);
    size_t numberOfChannels = bufferToFill.buffer->getNumChannels();

    switch (mode)
    {
    case Mode::Sine1kHz:
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            const float currentSample = (float) std::sin (currentAngle);
            currentAngle += angleDelta;
            prepared[sample] =  currentSample * level;
        }
        break;
    case Mode::White:
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            const float currentSample = dis(gen);
            prepared[sample] =  currentSample * level;
        }
        break;
    }

    for (size_t channel = 0; channel < numberOfChannels; channel++)
    {
        if (channel >= channels.size() || !channels[channel])

            continue;
        float* const buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

        memcpy(buffer, prepared.data(), bufferToFill.numSamples * sizeof(float));
    }
}
