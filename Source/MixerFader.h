#pragma once

#include <functional>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"

#include "Player.h"
#include "VolumeSlider.h"
#include "LevelMeter.h"

class MixerFader 
	: public Component
	, public ButtonListener
	, public juce::Slider::Listener
	, public MixerControlableChangeListener
	, public Timer
{
public:

	typedef std::function<void(bool)> SoloChangedCallback;
	typedef std::function<void()> ResizeCallback;

	MixerFader(MixerControlable* mainControlable, std::vector<MixerControlable*> subControlable, bool panEnabled, ResizeCallback resizeCallback);
	~MixerFader();

	void paint(Graphics&);
	void resized();

	void buttonClicked(Button* buttonThatWasClicked);
	void sliderValueChanged(Slider* sliderThatWasMoved);

	float getValue();
	void setValue(float value);

	void setMute(bool mute);
	void setSolo(bool solo);

	void setColor(Colour color);

	void setLabel(String text);

	void setMixSettings(std::vector<MixerControlable*> mixSettings);

	virtual void gainChanged(float gain) override;

	virtual void panChanged(float pan) override;

	virtual void soloChanged(bool solo) override;

	virtual void muteChanged(bool mute) override;

	virtual void timerCallback() override;

private:
	ScopedPointer<Label> m_label;
	ScopedPointer<TextButton> m_soloButton;
	ScopedPointer<TextButton> m_muteButton;
	ScopedPointer<ArrowButton> m_expandButton;
	ScopedPointer<Slider> m_panSlider;
	ScopedPointer<LevelMeter> m_levelMeter;
	ScopedPointer<VolumeSlider> m_volumeSlider;

	ResizeCallback m_resizeCallback;

	std::vector<std::unique_ptr<MixerFader>> m_subfaders;

	MixerControlable* m_mixerControlable;

	Colour m_color;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerFader)
};
