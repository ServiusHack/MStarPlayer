#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

class ResizingComponent : public juce::Component
{
public:
    virtual void childBoundsChanged(juce::Component* child) override;
};
