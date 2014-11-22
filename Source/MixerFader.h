#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

#include "Player.h"
#include "VolumeSlider.h"

class MixerFader 
	: public Component
	, public ButtonListener
	, public juce::Slider::Listener
	, public MixerControlableChangeListener
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

private:
	ScopedPointer<Label> m_label;
	ScopedPointer<TextButton> m_soloButton;
	ScopedPointer<TextButton> m_muteButton;
	ScopedPointer<ArrowButton> m_expandButton;
	ScopedPointer<Slider> m_panSlider;
	ScopedPointer<VolumeSlider> m_volumeSlider;

	ResizeCallback m_resizeCallback;

	std::vector<MixerFader*> m_subfaders;

	MixerControlable* m_mixerControlable;

	Colour m_color;
};
