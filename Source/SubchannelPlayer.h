#pragma once

#include "MixerControlable.h"

#include "Track.h"

/**
	A mixer controlable which might contain subordinate mixer controlables.

	This is only the case for Player and could thus be moved to there.
	But then we have a cyclic dependency between MixerComponent and Player.
*/
class SubchannelPlayer : public MixerControlable {
public:
	virtual void SetChannelCountChangedCallback(const Track::ChannelCountChangedCallback& callback) = 0;

	virtual std::vector<MixerControlable*> getSubMixerControlables() const = 0;
};
