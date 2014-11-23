#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LevelMeter : public Component
{
  public:
	  LevelMeter();

	  void setVolume(float volume);

	  virtual void paint(Graphics& g) override;

private:
	float m_volume;
};