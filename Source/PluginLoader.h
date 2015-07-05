#pragma once

#include <vector>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"

#include "MyMultiDocumentPanel.h"

#include "PluginInterface.h"

class PluginLoader {

public:
    PluginLoader(MyMultiDocumentPanel* content);
    ~PluginLoader();

    void configure(size_t index);

    void playingStateChanged(const char* playerName, bool isPlaying);

    void nextEntrySelected(const char* playerName);

    void previousEntrySelected(const char* playerName);

    void playlistEntrySelected(const char* playerName, size_t entryIndex);

    void trackVolumeChanged(const char* playerName, const char* trackName, float volume);

    void positionChanged(const char* playerName, double position);

    size_t count();

    String pluginName(size_t index);

    void saveConfigurations(XmlElement* element);

    void loadConfigurations(XmlElement* element);

private:

    struct Plugin {
        Plugin();
        Plugin(Plugin&& other);
        String name;
        std::unique_ptr<DynamicLibrary> dynamicLibrary;
        PluginInterface::InitFunction initFunction;
        PluginInterface::PlayingStateChangedFunction playingStateChangedFunction;
        PluginInterface::NextEntrySelectedFunction nextEntrySelectedFunction;
        PluginInterface::PreviousEntrySelectedFunction previousEntrySelectedFunction;
        PluginInterface::PlaylistEntrySelectedFunction playlistEntrySelectedFunction;
        PluginInterface::TrackVolumeChangedFunction trackVolumeChangedFunction;
        PluginInterface::PositionChangedFunction positionChangedFunction;
        PluginInterface::ConfigureFunction configureFunction;
        PluginInterface::ShutdownFunction shutdownFunction;
        PluginInterface::LoadConfigurationFunction loadConfigurationFunction;
        PluginInterface::GetConfigurationFunction getConfigurationFunction;
        PluginInterface::FreeConfigurationTextFunction freeConfigurationTextFunction;
    };

    std::vector<Plugin> plugins;

};