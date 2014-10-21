/*
  ==============================================================================

    Player.h
    Created: 14 Jan 2014 4:31:33pm
    Author:  User

  ==============================================================================
*/

#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

class Player : public Component {

public:
	virtual void setGain(float gain) = 0;
	
	virtual XmlElement* saveToXml() const = 0;

	virtual void setOutputChannels(int outputChannels) = 0;

};



#endif  // PLAYER_H_INCLUDED
