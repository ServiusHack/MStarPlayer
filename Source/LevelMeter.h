#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
	Draw the volume as a level meter.
*/
class LevelMeter : public Component
{
  public:
	  LevelMeter();

	  void setVolume(float volume);

	  virtual void paint(Graphics& g) override;

private:
	const AffineTransform m_rotationTransformation;
	float m_volume;
};