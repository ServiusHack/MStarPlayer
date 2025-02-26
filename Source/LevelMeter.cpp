#include "LevelMeter.h"

#include "juce_audio_devices/juce_audio_devices.h"

#include "DefaultLookAndFeel.h"

namespace
{
constexpr float Epsilon = 0.001f;
}

LevelMeter::LevelMeter()
    : m_volume(0.0f)
    , m_peak(0.0f)
    , m_peakDurationLeft(0)
    , m_rotationTransformation(juce::AffineTransform::rotation(-90.0f * juce::MathConstants<float>::pi / 180.0f))
{
}

void LevelMeter::setVolume(float volume)
{
    if (volume > m_peak || m_peakDurationLeft-- <= 0)
    {
        m_peak = volume;
        m_peakDurationLeft = LevelMeter::refreshRate;
    }
    const bool valueChanged = std::abs(m_volume - volume) > Epsilon;
    if (valueChanged || m_peakDurationLeft == LevelMeter::refreshRate)
    {
        m_volume = volume;
        repaint();
    }
}

void LevelMeter::paint(juce::Graphics& g)
{
    g.saveState();
    g.addTransform(m_rotationTransformation.translated(0.0f, static_cast<float>(getHeight())));
    static_cast<DefaultLookAndFeel&>(juce::LookAndFeel::getDefaultLookAndFeel())
        .drawLevelMeter(g, getHeight(), getWidth(), m_volume, m_peak);
    g.restoreState();
}
