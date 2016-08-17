#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ResizingComponent : public Component
{
public:
    virtual void childBoundsChanged(Component* child) override;
};