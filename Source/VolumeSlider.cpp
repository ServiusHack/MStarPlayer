#include "VolumeSlider.h"

#include "juce_audio_basics/juce_audio_basics.h"

VolumeSlider::VolumeSlider()
    : m_snapDistance(0.2)
{
    setRange(0.0, juce::Decibels::decibelsToGain(10.0), 0.001);
    setSkewFactor(0.3);
    setSliderStyle(juce::Slider::LinearVertical);
    setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
}

void VolumeSlider::paint(juce::Graphics& g)
{
    const int indent = juce::LookAndFeel::getDefaultLookAndFeel().getSliderThumbRadius(*this);
    const float zeroPosition = static_cast<float>((1.0 - valueToProportionOfLength(1.0)) * (getHeight() - 2 * indent));

    g.setColour(juce::Colours::black);
    g.drawLine(0.0f, indent + zeroPosition, static_cast<float>(getWidth()), indent + zeroPosition);

    juce::Slider::paint(g);
}

double VolumeSlider::snapValue(double attemptedValue, juce::Slider::DragMode /*dragMode*/)
{
    if (std::abs(1.0 - attemptedValue) < m_snapDistance)
        return 1.0;
    else
        return attemptedValue;
}
