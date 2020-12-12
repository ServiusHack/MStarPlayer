#include "PluginLoader.h"

#include <memory>

#include "PlayerComponent.h"

#include "PluginInterface.h"

static MyMultiDocumentPanel* component;

PlayerComponent* getPlayer(const char* playerName)
{
    const int components_count = component->getNumDocuments();
    for (int i = 0; i < components_count; ++i)
    {
        PlayerComponent* player = static_cast<PlayerComponent*>(component->getDocument(i));
        if (player->Component::getName() == playerName)
            return player;
    }

    return nullptr;
}

MixerControlable* getTrack(const PlayerComponent* player, const char* trackName)
{
    auto subMixerControlables = player->getSubMixerControlables();
    auto hit = std::find_if(subMixerControlables.begin(),
        subMixerControlables.end(),
        [trackName](const MixerControlable* controlable) { return controlable->getName() == trackName; });

    if (hit == subMixerControlables.end())
        return nullptr;

    return *hit;
}

void listPlayersV1(PluginInterface::V1::ListPlayersCallbackFunction callback, void* userData)
{
    const int components_count = component->getNumDocuments();
    std::vector<juce::String> names;
    for (int i = 0; i < components_count; ++i)
    {
        const PlayerComponent* player = static_cast<PlayerComponent*>(component->getDocument(i));
        callback(player->Component::getName().toRawUTF8(), userData);
    }
}

void listPlayersV2(PluginInterface::V2::ListPlayersCallbackFunction callback, void* userData)
{
    const int components_count = component->getNumDocuments();
    std::vector<juce::String> names;
    for (int i = 0; i < components_count; ++i)
    {
        const PlayerComponent* player = static_cast<PlayerComponent*>(component->getDocument(i));
        callback(player->Component::getName().toRawUTF8(), userData);
    }
}

void listPlayersV3(PluginInterface::V3::ListPlayersCallbackFunction callback, void* userData)
{
    const int components_count = component->getNumDocuments();
    std::vector<juce::String> names;
    for (int i = 0; i < components_count; ++i)
    {
        const PlayerComponent* player = static_cast<PlayerComponent*>(component->getDocument(i));
        callback(player->Component::getName().toRawUTF8(), userData);
    }
}

void play(const char* playerName)
{
    const juce::MessageManagerLock mmLock;
    PlayerComponent* player = getPlayer(playerName);
    if (player != nullptr)
        player->play();
}

void stop(const char* playerName)
{
    const juce::MessageManagerLock mmLock;
    PlayerComponent* player = getPlayer(playerName);
    if (player != nullptr)
        player->stop();
}

void nextEntry(const char* playerName)
{
    const juce::MessageManagerLock mmLock;
    PlayerComponent* player = getPlayer(playerName);
    if (player != nullptr)
        player->nextEntry();
}

void previousEntry(const char* playerName)
{
    const juce::MessageManagerLock mmLock;
    PlayerComponent* player = getPlayer(playerName);
    if (player != nullptr)
        player->previousEntry();
}

void listTracksV1(const char* playerName, PluginInterface::V1::ListTracksCallbackFunction callback, void* userData)
{
    const PlayerComponent* player = getPlayer(playerName);

    std::vector<MixerControlable*> subMixerControlables = player->getSubMixerControlables();
    for (const MixerControlable* const subMixerControlable : subMixerControlables)
    {
        callback(subMixerControlable->getName().toRawUTF8(), userData);
    }
}

void listTracksV2(const char* playerName, PluginInterface::V2::ListTracksCallbackFunction callback, void* userData)
{
    const PlayerComponent* player = getPlayer(playerName);

    std::vector<MixerControlable*> subMixerControlables = player->getSubMixerControlables();
    for (const MixerControlable* const subMixerControlable : subMixerControlables)
    {
        callback(subMixerControlable->getName().toRawUTF8(), userData);
    }
}

void listTracksV3(const char* playerName, PluginInterface::V3::ListTracksCallbackFunction callback, void* userData)
{
    const PlayerComponent* player = getPlayer(playerName);

    std::vector<MixerControlable*> subMixerControlables = player->getSubMixerControlables();
    for (const MixerControlable* const subMixerControlable : subMixerControlables)
    {
        callback(subMixerControlable->getName().toRawUTF8(), userData);
    }
}

void playerVolume(const char* playerName, float volume)
{
    const juce::MessageManagerLock mmLock;
    PlayerComponent* player = getPlayer(playerName);
    if (player == nullptr)
        return;

    player->setGain(volume);
}

void trackVolume(const char* playerName, const char* trackName, float volume)
{
    const juce::MessageManagerLock mmLock;
    PlayerComponent* player = getPlayer(playerName);
    if (player == nullptr)
        return;

    MixerControlable* track = getTrack(player, trackName);
    if (track == nullptr)
        return;

    track->setGain(volume);
}

PluginLoader::PluginLoader(MyMultiDocumentPanel* pComponent)
{
    component = pComponent;

    juce::RangedDirectoryIterator directoryIterator(
        juce::File::getSpecialLocation(juce::File::currentExecutableFile).getSiblingFile("plugins"), false, "*.dll");
    std::vector<juce::File> dlls;

    struct LoadFailure
    {
        juce::String file;
        juce::String error;
    };
    juce::Array<LoadFailure> failedPlugins;
    for (juce::DirectoryEntry dirEntry : directoryIterator)
    {
        auto dynamicLibrary = std::make_unique<juce::DynamicLibrary>(dirEntry.getFile().getFullPathName());

        if (dynamicLibrary->getNativeHandle() == nullptr)
        {
            failedPlugins.add({dirEntry.getFile().getFileName(), "Unable to open the plugin."});
            continue;
        }

        auto versionFunction
            = reinterpret_cast<PluginInterface::MStarPluginVersion>(dynamicLibrary->getFunction("mstarPluginVersion"));

        if (!versionFunction)
        {
            failedPlugins.add({dirEntry.getFile().getFileName(),
                "Function 'mstarPluginVersion' not found. Is this a M*Player plugin?"});
            continue;
        }

        int version = versionFunction();
        switch (version)
        {
        case 1:
        {
            auto loadResult = loadPluginV1(*dynamicLibrary);
            switch (loadResult.index())
            {
            case 0:
            {
                PluginV1 plugin = std::move(std::get<0>(loadResult));
                plugin.name = dirEntry.getFile().getFileNameWithoutExtension().toStdString();
                plugin.dynamicLibrary = std::move(dynamicLibrary);
                pluginsV1.emplace_back(std::move(plugin));
                break;
            }
            case 1:
            {
                failedPlugins.add({dirEntry.getFile().getFileName(), std::get<1>(loadResult)});
                break;
            }
            default:
                std::terminate();
            }
            break;
        }
        case 2:
        {
            auto loadResult = loadPluginV2(*dynamicLibrary);
            switch (loadResult.index())
            {
            case 0:
            {
                PluginV2 plugin = std::move(std::get<0>(loadResult));
                plugin.name = dirEntry.getFile().getFileNameWithoutExtension().toStdString();
                plugin.dynamicLibrary = std::move(dynamicLibrary);
                pluginsV2.emplace_back(std::move(plugin));
                break;
            }
            case 1:
            {
                failedPlugins.add({dirEntry.getFile().getFileName(), std::get<1>(loadResult)});
                break;
            }
            default:
                std::terminate();
            }
            break;
        }
        case 3:
        {
            auto loadResult = loadPluginV3(*dynamicLibrary);
            switch (loadResult.index())
            {
            case 0:
            {
                PluginV3 plugin = std::move(std::get<0>(loadResult));
                plugin.name = dirEntry.getFile().getFileNameWithoutExtension().toStdString();
                plugin.dynamicLibrary = std::move(dynamicLibrary);
                pluginsV3.emplace_back(std::move(plugin));
                break;
            }
            case 1:
            {
                failedPlugins.add({dirEntry.getFile().getFileName(), std::get<1>(loadResult)});
                break;
            }
            default:
                std::terminate();
            }
            break;
        }
        default:
            failedPlugins.add({dirEntry.getFile().getFileName(),
                "Plugin has version " + juce::String(version) + " but M*Player requires version 1, 2 or 3."});
            continue;
        }
    }

    {
        PluginInterface::V1::Init init;
        init.listPlayers = &listPlayersV1;
        init.listTracks = &listTracksV1;
        init.play = &play;
        init.stop = &stop;
        init.next = &nextEntry;
        init.previous = &previousEntry;
        init.setTrackVolume = &trackVolume;

        for (const auto& plugin : pluginsV1)
        {
            plugin.initFunction(init);
        }
    }

    {
        PluginInterface::V2::Init init;
        init.listPlayers = &listPlayersV2;
        init.listTracks = &listTracksV2;
        init.play = &play;
        init.stop = &stop;
        init.next = &nextEntry;
        init.previous = &previousEntry;
        init.setTrackVolume = &trackVolume;

        for (const auto& plugin : pluginsV2)
        {
            plugin.initFunction(init);
        }
    }

    {
        PluginInterface::V3::Init init;
        init.listPlayers = &listPlayersV2;
        init.listTracks = &listTracksV2;
        init.play = &play;
        init.stop = &stop;
        init.next = &nextEntry;
        init.previous = &previousEntry;
        init.setPlayerVolume = &playerVolume;
        init.setTrackVolume = &trackVolume;

        for (const auto& plugin : pluginsV3)
        {
            plugin.initFunction(init);
        }
    }

    if (failedPlugins.size() > 0)
    {
        juce::String lines;
        for (const LoadFailure& failure : failedPlugins)
        {
            lines += "\n" + failure.file + " (" + failure.error + ")";
        }
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            TRANS("Problems while loading plugins"),
            TRANS("The following plugins failed to load:\n") + lines);
    }
}

PluginLoader::~PluginLoader()
{
    for (const auto& plugin : pluginsV1)
    {
        plugin.shutdownFunction();
    }
    for (const auto& plugin : pluginsV2)
    {
        plugin.shutdownFunction();
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.shutdownFunction();
    }
}

size_t PluginLoader::count()
{
    return pluginsV1.size() + pluginsV2.size() + pluginsV3.size();
}

juce::String PluginLoader::pluginName(size_t index)
{
    if (index < pluginsV1.size())
        return pluginsV1[index].name;

    index -= pluginsV1.size();

    if (index < pluginsV2.size())
        return pluginsV2[index].name;

    index -= pluginsV2.size();

    return pluginsV3[index].name;
}

void PluginLoader::configure(size_t index)
{
    if (index < pluginsV1.size())
        pluginsV1[index].configureFunction();

    index -= pluginsV1.size();

    if (index < pluginsV2.size())
        pluginsV2[index].configureFunction();

    index -= pluginsV2.size();

    if (index < pluginsV3.size())
        pluginsV3[index].configureFunction();
}

void PluginLoader::playingStateChanged(const char* playerName, bool isPlaying)
{
    for (const auto& plugin : pluginsV1)
    {
        plugin.playingStateChangedFunction(playerName, isPlaying);
    }
    for (const auto& plugin : pluginsV2)
    {
        plugin.playingStateChangedFunction(playerName, isPlaying);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.playingStateChangedFunction(playerName, isPlaying);
    }
}

void PluginLoader::nextEntrySelected(const char* playerName)
{
    for (const auto& plugin : pluginsV1)
    {
        plugin.nextEntrySelectedFunction(playerName);
    }
    for (const auto& plugin : pluginsV2)
    {
        plugin.nextEntrySelectedFunction(playerName);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.nextEntrySelectedFunction(playerName);
    }
}

void PluginLoader::previousEntrySelected(const char* playerName)
{
    for (const auto& plugin : pluginsV1)
    {
        plugin.previousEntrySelectedFunction(playerName);
    }
    for (const auto& plugin : pluginsV2)
    {
        plugin.previousEntrySelectedFunction(playerName);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.previousEntrySelectedFunction(playerName);
    }
}

void PluginLoader::playlistEntrySelected(
    const char* playerName, size_t entryIndex, const char* playlistEntryName, double duration)
{
    for (const auto& plugin : pluginsV1)
    {
        plugin.playlistEntrySelectedFunction(playerName, entryIndex);
    }
    for (const auto& plugin : pluginsV2)
    {
        plugin.playlistEntrySelectedFunction(playerName, entryIndex, playlistEntryName, duration);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.playlistEntrySelectedFunction(playerName, entryIndex, playlistEntryName, duration);
    }
}

void PluginLoader::playlistEntryNameChanged(const char* playerName, size_t entryIndex, const char* playlistEntryName)
{
    for (const auto& plugin : pluginsV2)
    {
        plugin.playlistEntryNameChangedFunction(playerName, entryIndex, playlistEntryName);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.playlistEntryNameChangedFunction(playerName, entryIndex, playlistEntryName);
    }
}

void PluginLoader::playlistEntryDurationChanged(const char* playerName, size_t entryIndex, double duration)
{
    for (const auto& plugin : pluginsV2)
    {
        plugin.playlistEntryDurationChangedFunction(playerName, entryIndex, duration);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.playlistEntryDurationChangedFunction(playerName, entryIndex, duration);
    }
}

void PluginLoader::trackVolumeChanged(const char* playerName, const char* trackName, float volume)
{
    for (const auto& plugin : pluginsV1)
    {
        plugin.trackVolumeChangedFunction(playerName, trackName, volume);
    }
    for (const auto& plugin : pluginsV2)
    {
        plugin.trackVolumeChangedFunction(playerName, trackName, volume);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.trackVolumeChangedFunction(playerName, trackName, volume);
    }
}

void PluginLoader::positionChanged(const char* playerName, double position)
{
    for (const auto& plugin : pluginsV1)
    {
        plugin.positionChangedFunction(playerName, position);
    }
    for (const auto& plugin : pluginsV2)
    {
        plugin.positionChangedFunction(playerName, position);
    }
    for (const auto& plugin : pluginsV3)
    {
        plugin.positionChangedFunction(playerName, position);
    }
}

void PluginLoader::saveConfigurations(juce::XmlElement* pluginsElement)
{
    for (auto&& plugin : pluginsV1)
    {
        juce::XmlElement* pluginElement = new juce::XmlElement("Plugin");
        pluginElement->setAttribute("name", plugin.name);
        const char* data = plugin.getConfigurationFunction();
        pluginElement->addTextElement(juce::String::fromUTF8(data));
        plugin.freeConfigurationTextFunction(data);
        pluginsElement->addChildElement(pluginElement);
    }
    for (auto&& plugin : pluginsV2)
    {
        juce::XmlElement* pluginElement = new juce::XmlElement("Plugin");
        pluginElement->setAttribute("name", plugin.name);
        const char* data = plugin.getConfigurationFunction();
        pluginElement->addTextElement(juce::String::fromUTF8(data));
        plugin.freeConfigurationTextFunction(data);
        pluginsElement->addChildElement(pluginElement);
    }
    for (auto&& plugin : pluginsV3)
    {
        juce::XmlElement* pluginElement = new juce::XmlElement("Plugin");
        pluginElement->setAttribute("name", plugin.name);
        const char* data = plugin.getConfigurationFunction();
        pluginElement->addTextElement(juce::String::fromUTF8(data));
        plugin.freeConfigurationTextFunction(data);
        pluginsElement->addChildElement(pluginElement);
    }
}

void PluginLoader::loadConfigurations(juce::XmlElement* pluginsElement)
{
    for (int i = 0; i < pluginsElement->getNumChildElements(); ++i)
    {
        juce::XmlElement* pluginElement = pluginsElement->getChildElement(i);
        juce::String configurationText = pluginElement->getAllSubText();
        juce::String pluginName = pluginElement->getStringAttribute("name");
        auto it = std::find_if(pluginsV1.begin(), pluginsV1.end(), [pluginName](const PluginV1& plugin) {
            return plugin.name == pluginName;
        });
        if (it != pluginsV1.end())
        {
            it->loadConfigurationFunction(configurationText.toRawUTF8());
            continue;
        }
        auto it2 = std::find_if(pluginsV2.begin(), pluginsV2.end(), [pluginName](const PluginV2& plugin) {
            return plugin.name == pluginName;
        });
        if (it2 != pluginsV2.end())
        {
            it2->loadConfigurationFunction(configurationText.toRawUTF8());
        }
        auto it3 = std::find_if(pluginsV3.begin(), pluginsV3.end(), [pluginName](const PluginV3& plugin) {
            return plugin.name == pluginName;
        });
        if (it3 != pluginsV3.end())
        {
            it3->loadConfigurationFunction(configurationText.toRawUTF8());
        }
    }
}

std::variant<PluginLoader::PluginV1, std::string> PluginLoader::loadPluginV1(juce::DynamicLibrary& dynamicLibrary)
{
    PluginV1 plugin;

    plugin.initFunction = reinterpret_cast<PluginInterface::V1::InitFunction>(dynamicLibrary.getFunction("init"));
    if (!plugin.initFunction)
    {
        return "Missing function 'init'.";
    }

    plugin.playingStateChangedFunction = reinterpret_cast<PluginInterface::V1::PlayingStateChangedFunction>(
        dynamicLibrary.getFunction("playingStateChanged"));
    if (!plugin.playingStateChangedFunction)
    {
        return "Missing function 'playingStateChanged'.";
    }

    plugin.nextEntrySelectedFunction = reinterpret_cast<PluginInterface::V1::NextEntrySelectedFunction>(
        dynamicLibrary.getFunction("nextEntrySelected"));
    if (!plugin.nextEntrySelectedFunction)
    {
        return "Missing function 'nextEntrySelected'.";
    }

    plugin.previousEntrySelectedFunction = reinterpret_cast<PluginInterface::V1::PreviousEntrySelectedFunction>(
        dynamicLibrary.getFunction("previousEntrySelected"));
    if (!plugin.previousEntrySelectedFunction)
    {
        return "Missing function 'previousEntrySelected'.";
    }

    plugin.playlistEntrySelectedFunction = reinterpret_cast<PluginInterface::V1::PlaylistEntrySelectedFunction>(
        dynamicLibrary.getFunction("playlistEntrySelected"));
    if (!plugin.playlistEntrySelectedFunction)
    {
        return "Missing function 'playlistEntrySelected'.";
    }

    plugin.trackVolumeChangedFunction = reinterpret_cast<PluginInterface::V1::TrackVolumeChangedFunction>(
        dynamicLibrary.getFunction("trackVolumeChanged"));
    if (!plugin.trackVolumeChangedFunction)
    {
        return "Missing function 'trackVolumeChanged'.";
    }

    plugin.positionChangedFunction
        = reinterpret_cast<PluginInterface::V1::PositionChangedFunction>(dynamicLibrary.getFunction("positionChanged"));
    if (!plugin.positionChangedFunction)
    {
        return "Missing function 'positionChanged'.";
    }

    plugin.configureFunction
        = reinterpret_cast<PluginInterface::V1::ConfigureFunction>(dynamicLibrary.getFunction("configure"));
    if (!plugin.configureFunction)
    {
        return "Missing function 'configure'.";
    }

    plugin.shutdownFunction
        = reinterpret_cast<PluginInterface::V1::ShutdownFunction>(dynamicLibrary.getFunction("shutdown"));
    if (!plugin.shutdownFunction)
    {
        return "Missing function 'shutdown'.";
    }

    plugin.loadConfigurationFunction = reinterpret_cast<PluginInterface::V1::LoadConfigurationFunction>(
        dynamicLibrary.getFunction("loadConfiguration"));
    if (!plugin.loadConfigurationFunction)
    {
        return "Missing function 'loadConfiguration'.";
    }

    plugin.getConfigurationFunction = reinterpret_cast<PluginInterface::V1::GetConfigurationFunction>(
        dynamicLibrary.getFunction("getConfiguration"));
    if (!plugin.getConfigurationFunction)
    {
        return "Missing function 'getConfiguration'.";
    }

    plugin.freeConfigurationTextFunction = reinterpret_cast<PluginInterface::V1::FreeConfigurationTextFunction>(
        dynamicLibrary.getFunction("freeConfigurationText"));
    if (!plugin.freeConfigurationTextFunction)
    {
        return "Missing function 'freeConfigurationText'.";
    }

    return plugin;
}

std::variant<PluginLoader::PluginV2, std::string> PluginLoader::loadPluginV2(juce::DynamicLibrary& dynamicLibrary)
{
    PluginV2 plugin;

    plugin.initFunction = reinterpret_cast<PluginInterface::V2::InitFunction>(dynamicLibrary.getFunction("mstarInit"));
    if (!plugin.initFunction)
    {
        return "Missing function 'init'.";
    }

    plugin.playingStateChangedFunction = reinterpret_cast<PluginInterface::V2::PlayingStateChangedFunction>(
        dynamicLibrary.getFunction("mstarPlayingStateChanged"));
    if (!plugin.playingStateChangedFunction)
    {
        return "Missing function 'playingStateChanged'.";
    }

    plugin.nextEntrySelectedFunction = reinterpret_cast<PluginInterface::V2::NextEntrySelectedFunction>(
        dynamicLibrary.getFunction("mstarNextEntrySelected"));
    if (!plugin.nextEntrySelectedFunction)
    {
        return "Missing function 'nextEntrySelected'.";
    }

    plugin.previousEntrySelectedFunction = reinterpret_cast<PluginInterface::V2::PreviousEntrySelectedFunction>(
        dynamicLibrary.getFunction("mstarPreviousEntrySelected"));
    if (!plugin.previousEntrySelectedFunction)
    {
        return "Missing function 'previousEntrySelected'.";
    }

    plugin.playlistEntrySelectedFunction = reinterpret_cast<PluginInterface::V2::PlaylistEntrySelectedFunction>(
        dynamicLibrary.getFunction("mstarPlaylistEntrySelected"));
    if (!plugin.playlistEntrySelectedFunction)
    {
        return "Missing function 'playlistEntrySelected'.";
    }

    plugin.playlistEntryDurationChangedFunction
        = reinterpret_cast<PluginInterface::V2::PlaylistEntryDurationChangedFunction>(
            dynamicLibrary.getFunction("mstarPlaylistEntryDurationChanged"));
    if (!plugin.playlistEntryDurationChangedFunction)
    {
        return "Missing function 'playlistEntryDurationChanged'.";
    }

    plugin.playlistEntryNameChangedFunction = reinterpret_cast<PluginInterface::V2::PlaylistEntryNameChangedFunction>(
        dynamicLibrary.getFunction("mstarPlaylistEntryNameChanged"));
    if (!plugin.playlistEntrySelectedFunction)
    {
        return "Missing function 'playlistEntryNameChanged'.";
    }

    plugin.trackVolumeChangedFunction = reinterpret_cast<PluginInterface::V2::TrackVolumeChangedFunction>(
        dynamicLibrary.getFunction("mstarTrackVolumeChanged"));
    if (!plugin.trackVolumeChangedFunction)
    {
        return "Missing function 'trackVolumeChanged'.";
    }

    plugin.positionChangedFunction = reinterpret_cast<PluginInterface::V2::PositionChangedFunction>(
        dynamicLibrary.getFunction("mstarPositionChanged"));
    if (!plugin.positionChangedFunction)
    {
        return "Missing function 'positionChanged'.";
    }

    plugin.configureFunction
        = reinterpret_cast<PluginInterface::V2::ConfigureFunction>(dynamicLibrary.getFunction("mstarConfigure"));
    if (!plugin.configureFunction)
    {
        return "Missing function 'configure'.";
    }

    plugin.shutdownFunction
        = reinterpret_cast<PluginInterface::V2::ShutdownFunction>(dynamicLibrary.getFunction("mstarShutdown"));
    if (!plugin.shutdownFunction)
    {
        return "Missing function 'shutdown'.";
    }

    plugin.loadConfigurationFunction = reinterpret_cast<PluginInterface::V2::LoadConfigurationFunction>(
        dynamicLibrary.getFunction("mstarLoadConfiguration"));
    if (!plugin.loadConfigurationFunction)
    {
        return "Missing function 'loadConfiguration'.";
    }

    plugin.getConfigurationFunction = reinterpret_cast<PluginInterface::V2::GetConfigurationFunction>(
        dynamicLibrary.getFunction("mstarGetConfiguration"));
    if (!plugin.getConfigurationFunction)
    {
        return "Missing function 'getConfiguration'.";
    }

    plugin.freeConfigurationTextFunction = reinterpret_cast<PluginInterface::V2::FreeConfigurationTextFunction>(
        dynamicLibrary.getFunction("mstarFreeConfigurationText"));
    if (!plugin.freeConfigurationTextFunction)
    {
        return "Missing function 'freeConfigurationText'.";
    }

    return plugin;
}

std::variant<PluginLoader::PluginV3, std::string> PluginLoader::loadPluginV3(juce::DynamicLibrary& dynamicLibrary)
{
    PluginV3 plugin;

    plugin.initFunction = reinterpret_cast<PluginInterface::V3::InitFunction>(dynamicLibrary.getFunction("mstarInit"));
    if (!plugin.initFunction)
    {
        return "Missing function 'init'.";
    }

    plugin.playingStateChangedFunction = reinterpret_cast<PluginInterface::V3::PlayingStateChangedFunction>(
        dynamicLibrary.getFunction("mstarPlayingStateChanged"));
    if (!plugin.playingStateChangedFunction)
    {
        return "Missing function 'playingStateChanged'.";
    }

    plugin.nextEntrySelectedFunction = reinterpret_cast<PluginInterface::V3::NextEntrySelectedFunction>(
        dynamicLibrary.getFunction("mstarNextEntrySelected"));
    if (!plugin.nextEntrySelectedFunction)
    {
        return "Missing function 'nextEntrySelected'.";
    }

    plugin.previousEntrySelectedFunction = reinterpret_cast<PluginInterface::V3::PreviousEntrySelectedFunction>(
        dynamicLibrary.getFunction("mstarPreviousEntrySelected"));
    if (!plugin.previousEntrySelectedFunction)
    {
        return "Missing function 'previousEntrySelected'.";
    }

    plugin.playlistEntrySelectedFunction = reinterpret_cast<PluginInterface::V3::PlaylistEntrySelectedFunction>(
        dynamicLibrary.getFunction("mstarPlaylistEntrySelected"));
    if (!plugin.playlistEntrySelectedFunction)
    {
        return "Missing function 'playlistEntrySelected'.";
    }

    plugin.playlistEntryDurationChangedFunction
        = reinterpret_cast<PluginInterface::V3::PlaylistEntryDurationChangedFunction>(
            dynamicLibrary.getFunction("mstarPlaylistEntryDurationChanged"));
    if (!plugin.playlistEntryDurationChangedFunction)
    {
        return "Missing function 'playlistEntryDurationChanged'.";
    }

    plugin.playlistEntryNameChangedFunction = reinterpret_cast<PluginInterface::V3::PlaylistEntryNameChangedFunction>(
        dynamicLibrary.getFunction("mstarPlaylistEntryNameChanged"));
    if (!plugin.playlistEntrySelectedFunction)
    {
        return "Missing function 'playlistEntryNameChanged'.";
    }

    plugin.playerVolumeChangedFunction = reinterpret_cast<PluginInterface::V3::PlayerVolumeChangedFunction>(
        dynamicLibrary.getFunction("mstarPlayerVolumeChanged"));
    if (!plugin.playerVolumeChangedFunction)
    {
        return "Missing function 'playerVolumeChanged'.";
    }

    plugin.trackVolumeChangedFunction = reinterpret_cast<PluginInterface::V3::TrackVolumeChangedFunction>(
        dynamicLibrary.getFunction("mstarTrackVolumeChanged"));
    if (!plugin.trackVolumeChangedFunction)
    {
        return "Missing function 'trackVolumeChanged'.";
    }

    plugin.positionChangedFunction = reinterpret_cast<PluginInterface::V3::PositionChangedFunction>(
        dynamicLibrary.getFunction("mstarPositionChanged"));
    if (!plugin.positionChangedFunction)
    {
        return "Missing function 'positionChanged'.";
    }

    plugin.configureFunction
        = reinterpret_cast<PluginInterface::V3::ConfigureFunction>(dynamicLibrary.getFunction("mstarConfigure"));
    if (!plugin.configureFunction)
    {
        return "Missing function 'configure'.";
    }

    plugin.shutdownFunction
        = reinterpret_cast<PluginInterface::V3::ShutdownFunction>(dynamicLibrary.getFunction("mstarShutdown"));
    if (!plugin.shutdownFunction)
    {
        return "Missing function 'shutdown'.";
    }

    plugin.loadConfigurationFunction = reinterpret_cast<PluginInterface::V3::LoadConfigurationFunction>(
        dynamicLibrary.getFunction("mstarLoadConfiguration"));
    if (!plugin.loadConfigurationFunction)
    {
        return "Missing function 'loadConfiguration'.";
    }

    plugin.getConfigurationFunction = reinterpret_cast<PluginInterface::V3::GetConfigurationFunction>(
        dynamicLibrary.getFunction("mstarGetConfiguration"));
    if (!plugin.getConfigurationFunction)
    {
        return "Missing function 'getConfiguration'.";
    }

    plugin.freeConfigurationTextFunction = reinterpret_cast<PluginInterface::V3::FreeConfigurationTextFunction>(
        dynamicLibrary.getFunction("mstarFreeConfigurationText"));
    if (!plugin.freeConfigurationTextFunction)
    {
        return "Missing function 'freeConfigurationText'.";
    }

    return plugin;
}

PluginLoader::PluginV1::PluginV1() {}

PluginLoader::PluginV1::PluginV1(PluginV1&& other)
    : name(std::move(other.name))
    , dynamicLibrary(std::move(other.dynamicLibrary))
    , initFunction(other.initFunction)
    , playingStateChangedFunction(other.playingStateChangedFunction)
    , nextEntrySelectedFunction(other.nextEntrySelectedFunction)
    , previousEntrySelectedFunction(other.previousEntrySelectedFunction)
    , playlistEntrySelectedFunction(other.playlistEntrySelectedFunction)
    , trackVolumeChangedFunction(other.trackVolumeChangedFunction)
    , positionChangedFunction(other.positionChangedFunction)
    , configureFunction(other.configureFunction)
    , shutdownFunction(other.shutdownFunction)
    , loadConfigurationFunction(other.loadConfigurationFunction)
    , getConfigurationFunction(other.getConfigurationFunction)
    , freeConfigurationTextFunction(other.freeConfigurationTextFunction)
{
}

PluginLoader::PluginV2::PluginV2() {}

PluginLoader::PluginV2::PluginV2(PluginV2&& other) = default;

PluginLoader::PluginV3::PluginV3() {}

PluginLoader::PluginV3::PluginV3(PluginV3&& other) = default;
