#include "PanSlider.h"

#include "juce_audio_basics/juce_audio_basics.h"

PanSlider::PanSlider()
    : m_snapDistance(0.05)
{
    setRange(-1.0, 1.0, 0.01);
    setValue(0.0);
    setSliderStyle(juce::Slider::LinearHorizontal);
    setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    setTransform(juce::AffineTransform().scaled(0.7f));
}

void PanSlider::paint(juce::Graphics& g)
{
    const int indent = juce::LookAndFeel::getDefaultLookAndFeel().getSliderThumbRadius(*this);
    const float zeroPosition = static_cast<float>((1.0 - valueToProportionOfLength(0.0)) * (getWidth() - 2 * indent));

    g.setColour(juce::Colours::black);
    g.drawLine(indent + zeroPosition, 0.0f, indent + zeroPosition, static_cast<float>(getHeight()));

    juce::Slider::paint(g);
}

double PanSlider::snapValue(double attemptedValue, juce::Slider::DragMode /*dragMode*/)
{
    if (std::abs(0.0 - attemptedValue) < m_snapDistance)
        return 0.0;
    else
        return attemptedValue;
}
