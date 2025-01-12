#include "ChangeableArrowButton.h"

ChangeableArrowButton::ChangeableArrowButton(
    const juce::String& name, float arrowDirectionInRadians, juce::Colour arrowColour)
    : juce::Button(name)
    , colour(arrowColour)
{
    path.addTriangle(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
    path.applyTransform(
        juce::AffineTransform::rotation(juce::MathConstants<float>::pi * 2.0f * arrowDirectionInRadians, 0.5f, 0.5f));
}

void ChangeableArrowButton::paintButton(juce::Graphics& g, bool /*isMouseOverButton*/, bool isButtonDown)
{
    juce::Path p(path);

    const float offset = isButtonDown ? 1.0f : 0.0f;
    p.applyTransform(path.getTransformToScaleToFit(offset, offset, getWidth() - 3.0f, getHeight() - 3.0f, false));

    juce::DropShadow(juce::Colours::black.withAlpha(0.3f), isButtonDown ? 2 : 4, juce::Point<int>()).drawForPath(g, p);

    g.setColour(colour);
    g.fillPath(p);
}

void ChangeableArrowButton::setArrowDirection(float arrowDirectionInRadians)
{
    path.clear();
    path.addTriangle(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
    path.applyTransform(
        juce::AffineTransform::rotation(juce::MathConstants<float>::pi * 2.0f * arrowDirectionInRadians, 0.5f, 0.5f));
    repaint();
}
