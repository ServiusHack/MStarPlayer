#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "SubchannelPlayer.h"

class PlayerComponent
    : public juce::Component
    , public SubchannelPlayer
{
public:
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void nextEntry(bool onlyIfEntrySaysSo = false) = 0;
    virtual void previousEntry() = 0;
};
