#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Player.h"
#include "MixerFader.h"

class PlayerMixerFader : public MixerFader
{
public:
	PlayerMixerFader(Player* player, std::vector<MixerControlable*> subControlable, bool panEnabled, ResizeCallback resizeCallback);

	Player* getPlayer() const;

private:
	Player* player;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerMixerFader)
};