#include "ChangeableArrowButton.h"

ChangeableArrowButton::ChangeableArrowButton(const String& name, float arrowDirectionInRadians, Colour arrowColour)
    : Button(name)
    , colour(arrowColour)
{
    path.addTriangle(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
    path.applyTransform(AffineTransform::rotation(float_Pi * 2.0f * arrowDirectionInRadians, 0.5f, 0.5f));
}

void ChangeableArrowButton::paintButton(Graphics& g, bool /*isMouseOverButton*/, bool isButtonDown)
{
    Path p(path);

    const float offset = isButtonDown ? 1.0f : 0.0f;
    p.applyTransform(path.getTransformToScaleToFit(offset, offset, getWidth() - 3.0f, getHeight() - 3.0f, false));

    DropShadow(Colours::black.withAlpha(0.3f), isButtonDown ? 2 : 4, Point<int>()).drawForPath(g, p);

    g.setColour(colour);
    g.fillPath(p);
}

void ChangeableArrowButton::setArrowDirection(float arrowDirectionInRadians)
{
    path.clear();
    path.addTriangle(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
    path.applyTransform(AffineTransform::rotation(float_Pi * 2.0f * arrowDirectionInRadians, 0.5f, 0.5f));
    repaint();
}