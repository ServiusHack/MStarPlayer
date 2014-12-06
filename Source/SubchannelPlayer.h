#pragma once

#include "MixerControlable.h"

#include "Track.h"

class SubchannelPlayer : public MixerControlable {
public:
	virtual void SetChannelCountChangedCallback(Track::ChannelCountChangedCallback callback) = 0;

	virtual std::vector<MixerControlable*> getSubMixerControlables() const = 0;
};
