#include "LevelMeter.h"
#include "DefaultLookAndFeel.h"

LevelMeter::LevelMeter()
    : m_volume(0.0f)
    , m_peak(0.0f)
    , m_peakDurationLeft(0)
    , m_rotationTransformation(AffineTransform::rotation(-90.0f * float_Pi / 180.0f))
{
}

void LevelMeter::setVolume(float volume)
{
    if (volume > m_peak || m_peakDurationLeft-- <= 0)
    {
        m_peak = volume;
        m_peakDurationLeft = LevelMeter::refreshRate;
    }
    m_volume = volume;
    repaint();
}

void LevelMeter::paint(Graphics& g)
{
    g.saveState();
    g.addTransform(m_rotationTransformation.translated(0.0f, static_cast<float>(getHeight())));
    static_cast<DefaultLookAndFeel&>(LookAndFeel::getDefaultLookAndFeel()).drawLevelMeter(g, getHeight(), getWidth(), m_volume, m_peak);
    g.restoreState();
}