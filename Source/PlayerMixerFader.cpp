#include "PlayerMixerFader.h"

PlayerMixerFader::PlayerMixerFader(SubchannelPlayer* player, std::vector<MixerControlable*> subControlable, bool panEnabled, ResizeCallback resizeCallback)
    : MixerFader(player, subControlable, panEnabled, resizeCallback)
    , player(player)
{
    player->SetChannelCountChangedCallback([this]() {
        std::vector<MixerControlable*> mixSettings = this->player->getSubMixerControlables();

        setMixSettings(mixSettings);
    });
}

SubchannelPlayer* PlayerMixerFader::getPlayer() const
{
    return player;
};