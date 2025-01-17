#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

/* Slider to change panning.
 *
 * Its value ranges from -1 (full left) to 1 (full right). The UI snaps to 0
 * which means center.
 */
class PanSlider : public juce::Slider
{
public:
    PanSlider();

protected:
    // Distance in gain from 0 when to snap to 0.
    double m_snapDistance;

    // Slider
public:
    virtual void paint(juce::Graphics&) override;
    virtual double snapValue(double attemptedValue, juce::Slider::DragMode dragMode) override;
};
