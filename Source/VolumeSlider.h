#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

/* Slider to change a volume.
 *
 * Also called fader. It has a logarithmic scale with 0 dB at about 80% to which it snaps.
 * The slider ranges from -inf dB to +10 dB.
 */
class VolumeSlider : public juce::Slider
{
public:
    VolumeSlider();
    explicit VolumeSlider(const juce::String& componentName);

protected:
    // Distance in gain from 0 when to snap to 0.
    double m_snapDistance;

    // Slider
public:
    virtual void paint(juce::Graphics&) override;
    virtual double snapValue(double attemptedValue, juce::Slider::DragMode dragMode) override;
};
