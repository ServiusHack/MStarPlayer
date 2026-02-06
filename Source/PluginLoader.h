#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "MyMultiDocumentPanel.h"

#include "PluginInterfaceV1.h"
#include "PluginInterfaceV2.h"
#include "PluginInterfaceV3.h"

class PluginLoader
{

public:
    PluginLoader(MyMultiDocumentPanel* content);
    ~PluginLoader();

    void configure(size_t index);

    void playingStateChanged(const char* playerName, bool isPlaying);

    void nextEntrySelected(const char* playerName);

    void previousEntrySelected(const char* playerName);

    void playlistEntrySelected(const char* playerName, int entryIndex, const char* playlistEntryName, double duration);

    void playlistEntryNameChanged(const char* playerName, int entryIndex, const char* playlistEntryName);

    void playlistEntryDurationChanged(const char* playerName, int entryIndex, double duration);

    void trackVolumeChanged(const char* playerName, const char* trackName, float volume);

    void positionChanged(const char* playerName, double position);

    size_t count();

    juce::String pluginName(size_t index);

    void saveConfigurations(juce::XmlElement* element);

    void loadConfigurations(juce::XmlElement* element);

private:
    struct PluginV1
    {
        PluginV1();
        PluginV1(PluginV1&& other);
        juce::String name;
        std::unique_ptr<juce::DynamicLibrary> dynamicLibrary;
        PluginInterface::V1::InitFunction initFunction;
        PluginInterface::V1::PlayingStateChangedFunction playingStateChangedFunction;
        PluginInterface::V1::NextEntrySelectedFunction nextEntrySelectedFunction;
        PluginInterface::V1::PreviousEntrySelectedFunction previousEntrySelectedFunction;
        PluginInterface::V1::PlaylistEntrySelectedFunction playlistEntrySelectedFunction;
        PluginInterface::V1::TrackVolumeChangedFunction trackVolumeChangedFunction;
        PluginInterface::V1::PositionChangedFunction positionChangedFunction;
        PluginInterface::V1::ConfigureFunction configureFunction;
        PluginInterface::V1::ShutdownFunction shutdownFunction;
        PluginInterface::V1::LoadConfigurationFunction loadConfigurationFunction;
        PluginInterface::V1::GetConfigurationFunction getConfigurationFunction;
        PluginInterface::V1::FreeConfigurationTextFunction freeConfigurationTextFunction;
    };

    struct PluginV2
    {
        PluginV2();
        PluginV2(PluginV2&& other);
        juce::String name;
        std::unique_ptr<juce::DynamicLibrary> dynamicLibrary;
        PluginInterface::V2::InitFunction initFunction;
        PluginInterface::V2::PlayingStateChangedFunction playingStateChangedFunction;
        PluginInterface::V2::NextEntrySelectedFunction nextEntrySelectedFunction;
        PluginInterface::V2::PreviousEntrySelectedFunction previousEntrySelectedFunction;
        PluginInterface::V2::PlaylistEntrySelectedFunction playlistEntrySelectedFunction;
        PluginInterface::V2::PlaylistEntryDurationChangedFunction playlistEntryDurationChangedFunction;
        PluginInterface::V2::PlaylistEntryNameChangedFunction playlistEntryNameChangedFunction;
        PluginInterface::V2::TrackVolumeChangedFunction trackVolumeChangedFunction;
        PluginInterface::V2::PositionChangedFunction positionChangedFunction;
        PluginInterface::V2::ConfigureFunction configureFunction;
        PluginInterface::V2::ShutdownFunction shutdownFunction;
        PluginInterface::V2::LoadConfigurationFunction loadConfigurationFunction;
        PluginInterface::V2::GetConfigurationFunction getConfigurationFunction;
        PluginInterface::V2::FreeConfigurationTextFunction freeConfigurationTextFunction;
    };

    struct PluginV3
    {
        PluginV3();
        PluginV3(PluginV3&& other);
        juce::String name;
        std::unique_ptr<juce::DynamicLibrary> dynamicLibrary;
        PluginInterface::V3::InitFunction initFunction;
        PluginInterface::V3::PlayingStateChangedFunction playingStateChangedFunction;
        PluginInterface::V3::NextEntrySelectedFunction nextEntrySelectedFunction;
        PluginInterface::V3::PreviousEntrySelectedFunction previousEntrySelectedFunction;
        PluginInterface::V3::PlaylistEntrySelectedFunction playlistEntrySelectedFunction;
        PluginInterface::V3::PlaylistEntryDurationChangedFunction playlistEntryDurationChangedFunction;
        PluginInterface::V3::PlaylistEntryNameChangedFunction playlistEntryNameChangedFunction;
        PluginInterface::V3::PlayerVolumeChangedFunction playerVolumeChangedFunction;
        PluginInterface::V3::TrackVolumeChangedFunction trackVolumeChangedFunction;
        PluginInterface::V3::PositionChangedFunction positionChangedFunction;
        PluginInterface::V3::ConfigureFunction configureFunction;
        PluginInterface::V3::ShutdownFunction shutdownFunction;
        PluginInterface::V3::LoadConfigurationFunction loadConfigurationFunction;
        PluginInterface::V3::GetConfigurationFunction getConfigurationFunction;
        PluginInterface::V3::FreeConfigurationTextFunction freeConfigurationTextFunction;
    };

    std::variant<PluginV1, std::string> loadPluginV1(juce::DynamicLibrary& library);
    std::variant<PluginV2, std::string> loadPluginV2(juce::DynamicLibrary& library);
    std::variant<PluginV3, std::string> loadPluginV3(juce::DynamicLibrary& library);

    std::vector<PluginV1> pluginsV1;
    std::vector<PluginV2> pluginsV2;
    std::vector<PluginV3> pluginsV3;
};
