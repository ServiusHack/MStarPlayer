#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SubchannelPlayer.h"
#include "MixerFader.h"

/**
	MixerFader for a player.
*/
class PlayerMixerFader : public MixerFader
{
public:
	PlayerMixerFader(SubchannelPlayer* player, std::vector<MixerControlable*> subControlable, bool panEnabled, ResizeCallback resizeCallback);

	SubchannelPlayer* getPlayer() const;

private:
	SubchannelPlayer* player;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerMixerFader)
};