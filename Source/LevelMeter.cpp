#include "LevelMeter.h"

LevelMeter::LevelMeter()
	: m_volume(0.0f)
	, m_rotationTransformation(AffineTransform::rotation(-90.0f * float_Pi / 180.0f))
{
}

void LevelMeter::setVolume(float volume)
{
	m_volume = volume;
	repaint();
}

void LevelMeter::paint(Graphics& g)
{
	g.saveState();
	g.addTransform(m_rotationTransformation.translated(0.0f, static_cast<float>(getHeight())));
	LookAndFeel::getDefaultLookAndFeel().drawLevelMeter(g, getHeight(), getWidth(),m_volume);
	g.restoreState();
}