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

typedef std::function<void()> ShowEditDialogCallback;
typedef std::function<void()> ConfigureChannelsCallback;
typedef std::function<void()> ConfigureMidiCallback;
typedef std::function<void(PlayerType)> ChangePlayerTypeCallback;
typedef std::function<void(juce::String)> SetUserImageCallback;
}
