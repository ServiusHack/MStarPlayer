#include "LevelMeter.h"

LevelMeter::LevelMeter()
	: m_volume(0.0f)
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
	g.addTransform(AffineTransform::rotation(-90.0f * float_Pi / 180.0f).translated(0, getHeight()));
	LookAndFeel::getDefaultLookAndFeel().drawLevelMeter(g, getHeight(), getWidth(),m_volume);
	g.restoreState();
}