/*
  ==============================================================================

    Track.h
    Created: 21 Jan 2014 1:00:48am
    Author:  User

  ==============================================================================
*/

#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Track    : public Component
{
public:
    Track(bool stereo = true);
    ~Track();

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};


#endif  // TRACK_H_INCLUDED
