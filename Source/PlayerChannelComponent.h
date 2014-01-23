/*
  ==============================================================================

    PlayerChannelComponent.h
    Created: 13 Jan 2014 11:56:46pm
    Author:  User

  ==============================================================================
*/

#ifndef PLAYERCHANNELCOMPONENT_H_INCLUDED
#define PLAYERCHANNELCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class PlayerChannelComponent    : public Component
{
public:
    PlayerChannelComponent();
    ~PlayerChannelComponent();

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerChannelComponent)
};


#endif  // PLAYERCHANNELCOMPONENT_H_INCLUDED
