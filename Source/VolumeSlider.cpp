#include "VolumeSlider.h"

VolumeSlider::VolumeSlider()
	: m_snapDistance(0.2)
{
	setRange(0.0, Decibels::decibelsToGain(10.0), 0.001);
	setSkewFactor(0.3);
	setSliderStyle(Slider::LinearVertical);
	setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
}

void VolumeSlider::paint(Graphics& g)
{
	const int indent = LookAndFeel::getDefaultLookAndFeel().getSliderThumbRadius(*this);
	float zeroPosition = static_cast<float>((1.0 - valueToProportionOfLength(1.0)) * (getHeight() - 2 * indent));

	g.setColour(Colours::black);
	g.drawLine(0.0f, indent + zeroPosition, static_cast<float>(getWidth()), indent + zeroPosition);

	Slider::paint(g);
}

double VolumeSlider::snapValue(double attemptedValue, DragMode /*dragMode*/)
{
	if (std::abs(1.0 - attemptedValue) < m_snapDistance)
		return 1.0;
	else
		return attemptedValue;
}