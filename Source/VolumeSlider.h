#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class VolumeSlider : public Slider
{
public:
	VolumeSlider();

	virtual void paint(Graphics &) override;
	virtual double snapValue(double attemptedValue, DragMode dragMode) override;

protected:
	double m_snapDistance;
};