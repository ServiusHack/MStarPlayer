#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/* Slider to change a volume.
 *
 * Also called fader. It has a logarithmic scale with 0 dB at about 80% to which it snaps.
 * The slider ranges from -inf dB to +10 dB.
 */
class VolumeSlider : public Slider
{
public:
    VolumeSlider();
    explicit VolumeSlider(const String& componentName);

protected:
    // Distance in gain from 0 when to snap to 0.
    double m_snapDistance;

    // Slider
public:
    virtual void paint(Graphics&) override;
    virtual double snapValue(double attemptedValue, DragMode dragMode) override;
};
