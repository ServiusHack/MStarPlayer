#pragma once

namespace PluginInterface
{
    using MStarPluginVersion = int(*)();
    using ListPlayersCallbackFunction = void(*)(const char* player_name, void* userData);
    using ListPlayersFunction = void(*)(ListPlayersCallbackFunction callback, void* userData);
    using PlayFunction = void(*)(const char* player_name);
    using StopFunction = void(*)(const char* player_name);
    using NextEntryFunction = void(*)(const char* player_name);
    using PreviousEntryFunction = void(*)(const char* player_name);
    using ListTracksCallbackFunction = void(*)(const char* track_name, void* userData);
    using ListTracksFunction = void(*)(const char* player_name, ListTracksCallbackFunction callback, void* userData);
    using SetTrackVolumeFunction = void(*)(const char* player_name, const char* track_name, float volume);

    struct Init {
        ListPlayersFunction listPlayers;
        PlayFunction play;
        StopFunction stop;
        NextEntryFunction next;
        PreviousEntryFunction previous;
        ListTracksFunction listTracks;
        SetTrackVolumeFunction setTrackVolume;
    };

    using InitFunction = void(*)(const Init&);
    using PlayingStateChangedFunction = void(*)(const char* playerName, bool isPlaying);
    using NextEntrySelectedFunction = void(*)(const char* playerName);
    using PreviousEntrySelectedFunction = void(*)(const char* playerName);
    using PlaylistEntrySelectedFunction = void(*)(const char* playerName, int playlist_index);
    using TrackVolumeChangedFunction = void(*)(const char* playerName, const char* track_name, double volume);
    using PositionChangedFunction = void(*)(const char* playerName, double position);
    using ConfigureFunction = void(*)();
    using ShutdownFunction = void(*)();

    using LoadConfigurationFunction = void(*)(const char* configurationText);
    using GetConfigurationFunction = char*(*)();
    using FreeConfigurationTextFunction = void(*)(const char* configurationText);

}
