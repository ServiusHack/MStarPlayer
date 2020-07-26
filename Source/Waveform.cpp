#include "Waveform.h"
#include "DefaultLookAndFeel.h"

Waveform::Waveform(Colour color)
    : m_color(color)
    , m_audioThumbnail(nullptr)
{
    setBufferedToImage(true);
    setInterceptsMouseClicks(false, false);
}

void Waveform::setAudioThumbnail(AudioThumbnail* audioThumbnail)
{
    if (m_audioThumbnail == audioThumbnail)
        return;

    if (m_audioThumbnail != nullptr)
        m_audioThumbnail->removeChangeListener(this);

    m_audioThumbnail = audioThumbnail;

    m_audioThumbnail->addChangeListener(this);

    repaint();
}

void Waveform::paint(Graphics& g)
{
    g.setColour(m_color);
    m_audioThumbnail->drawChannels(g, getLocalBounds(), 0, m_audioThumbnail->getTotalLength(), 1.0f);
}

void Waveform::changeListenerCallback(ChangeBroadcaster* source)
{
    repaint();
}
