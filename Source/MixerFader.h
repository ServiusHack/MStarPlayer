#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

class MixerFader 
	: public Component
	, public ButtonListener
	, public juce::Slider::Listener
{
public:

	typedef std::function<void(float)> VolumeChangedCallback;
	typedef std::function<void(float)> PanChangedCallback;
	typedef std::function<void(bool)> SoloChangedCallback;
	typedef std::function<void(bool)> MuteChangedCallback;

	MixerFader(bool panEnabled, float gain, bool solo, bool mute, VolumeChangedCallback volumeCallback, PanChangedCallback panCallback, SoloChangedCallback soloCallback, MuteChangedCallback muteCallback);

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

private:
	ScopedPointer<Label> m_label;
	ScopedPointer<TextButton> m_soloButton;
	ScopedPointer<TextButton> m_muteButton;
	ScopedPointer<Slider> m_panSlider;
	ScopedPointer<Slider> m_volumeSlider;

	VolumeChangedCallback m_volumeCallback;
	PanChangedCallback m_panCallback;
	SoloChangedCallback m_soloCallback;
	MuteChangedCallback m_muteCallback;

	Colour m_color;
};
