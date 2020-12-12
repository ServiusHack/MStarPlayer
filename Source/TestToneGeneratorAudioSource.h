#pragma once

#include <random>

#include "juce_audio_basics/juce_audio_basics.h"

/**
        Audio source which generates test tones on selected output channels.
*/
class TestToneGeneratorAudioSource : public juce::AudioSource
{
public:
    enum class Mode
    {
        Sine1kHz,
        White
    };

    TestToneGeneratorAudioSource();

    void setMode(Mode mode);

    void setNumChannels(int numberOfChannels);
    void setEnable(int channel, bool enable);

    void setVolume(double volume);
    double getVolume() const;

    // AudioSource
public:
    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    virtual void releaseResources() override;
    virtual void getNextAudioBlock(const juce::AudioSourceChannelInfo&) override;

private:
    juce::CriticalSection m_lock;
    int m_samplesPerBlockExpected;
    double m_sampleRate;
    double currentAngle{0.0};
    double angleDelta;
    juce::Array<bool> channels;
    std::vector<float> prepared;
    std::uniform_real_distribution<float> dis;
    std::mt19937 gen;
    Mode mode{Mode::Sine1kHz};
    float level = 0.125f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestToneGeneratorAudioSource)
};
