#pragma once

#include <functional>

namespace InterPlayerCommunication
{
	enum class PlayerType {
		Jingle,
		Multitrack,
		Playlist
	};

	typedef std::function<void()> ShowEditDialogCallback;
	typedef std::function<void()> ConfigureChannelsCallback;
	typedef std::function<void(PlayerType)> ChangePlayerTypeCallback;
}
