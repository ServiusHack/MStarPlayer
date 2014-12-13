#pragma once

#include <functional>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerControlable.h"
#include "VolumeSlider.h"
#include "LevelMeter.h"

/**
	A set of controls for an player channel or output channel.

	This includes:
	- volume fader
	- level meter
	- solo button
	- mute button
	- pan slider (not fully implemented yet)
*/
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

	float getValue();
	void setValue(float value);

	void setMute(bool mute);
	void setSolo(bool solo);

	void setColor(const Colour& color);

	void setLabel(const String& text);

	void setMixSettings(std::vector<MixerControlable*> mixSettings);

// Component
public:
	virtual void paint(Graphics&) override;
	virtual void resized() override;

// ButtonListener
public:
	virtual void buttonClicked(Button* buttonThatWasClicked) override;

// Slider::Listener
public:
	virtual void sliderValueChanged(Slider* sliderThatWasMoved) override;

// MixerControlableChangeListener
public:
	virtual void gainChanged(float gain) override;

	virtual void panChanged(float pan) override;

	virtual void soloChanged(bool solo) override;

	virtual void muteChanged(bool mute) override;

	virtual void nameChanged(const String& name) override;

// Timer
public:
	virtual void timerCallback() override;

protected:
	ScopedPointer<VolumeSlider> m_volumeSlider;

private:
	ScopedPointer<Label> m_label;
	ScopedPointer<TextButton> m_soloButton;
	ScopedPointer<TextButton> m_muteButton;
	ScopedPointer<ArrowButton> m_expandButton;
	ScopedPointer<Slider> m_panSlider;
	ScopedPointer<LevelMeter> m_levelMeter;

	ResizeCallback m_resizeCallback;

	std::vector<std::unique_ptr<MixerFader>> m_subfaders;

	MixerControlable* m_mixerControlable;

	Colour m_color;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerFader)
};
