#pragma once

#include <functional>

#include "juce_core/juce_core.h"

/**
        Callbacks and enums for the communication between Player and the user interface classes.
*/
namespace InterPlayerCommunication
{
enum class PlayerType
{
    Jingle,
    Multitrack,
    Playlist
};

using ShowEditDialogCallback = std::function<void()>;
using ConfigureChannelsCallback = std::function<void()>;
using ConfigureMidiCallback = std::function<void()>;
using ChangePlayerTypeCallback = std::function<void(PlayerType)>;
using SetUserImageCallback = std::function<void(juce::String)>;
}
