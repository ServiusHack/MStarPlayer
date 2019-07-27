#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SubchannelPlayer.h"

class PlayerComponent
    : public Component
    , public SubchannelPlayer
{
public:
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void nextEntry(bool onlyIfEntrySaysSo = false) = 0;
    virtual void previousEntry() = 0;
};