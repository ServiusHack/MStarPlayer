#include "Waveform.h"
#include "DefaultLookAndFeel.h"

Waveform::Waveform(juce::Colour color)
    : m_color(color)
    , m_audioThumbnail(nullptr)
{
    setBufferedToImage(true);
    setInterceptsMouseClicks(false, false);
}

void Waveform::setAudioThumbnail(juce::AudioThumbnail* audioThumbnail)
{
    if (m_audioThumbnail == audioThumbnail)
        return;

    if (m_audioThumbnail != nullptr)
        m_audioThumbnail->removeChangeListener(this);

    m_audioThumbnail = audioThumbnail;

    m_audioThumbnail->addChangeListener(this);

    repaint();
}

void Waveform::paint(juce::Graphics& g)
{
    g.setColour(m_color);
    m_audioThumbnail->drawChannels(g, getLocalBounds(), 0, m_audioThumbnail->getTotalLength(), 1.0f);
}

void Waveform::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
}
