#include "PluginLoader.h"

#include <memory>

#include "Player.h"

static MyMultiDocumentPanel* component;

Player* getPlayer(const char* playerName)
{
    const int components_count = component->getNumDocuments();
    for (int i = 0; i < components_count; ++i)
    {
        Player* player = static_cast<Player*>(component->getDocument(i));
        if (player->getName() == playerName)
            return player;
    }

    return nullptr;
}

MixerControlable* getTrack(const Player* player, const char* trackName)
{
    auto subMixerControlables = player->getSubMixerControlables();
    auto hit = std::find_if(subMixerControlables.begin(),
        subMixerControlables.end(),
        [trackName](const MixerControlable* controlable) { return controlable->getName() == trackName; });

    if (hit == subMixerControlables.end())
        return nullptr;

    return *hit;
}

void listPlayers(PluginInterface::ListPlayersCallbackFunction callback, void* userData)
{
    const int components_count = component->getNumDocuments();
    std::vector<String> names;
    for (int i = 0; i < components_count; ++i)
    {
        const Player* player = static_cast<Player*>(component->getDocument(i));
        callback(player->getName().toRawUTF8(), userData);
    }
}

void play(const char* playerName)
{
    const MessageManagerLock mmLock;
    Player* player = getPlayer(playerName);
    if (player != nullptr)
        player->play();
}

void stop(const char* playerName)
{
    const MessageManagerLock mmLock;
    Player* player = getPlayer(playerName);
    if (player != nullptr)
        player->stop();
}

void nextEntry(const char* playerName)
{
    const MessageManagerLock mmLock;
    Player* player = getPlayer(playerName);
    if (player != nullptr)
        player->nextEntry();
}

void previousEntry(const char* playerName)
{
    const MessageManagerLock mmLock;
    Player* player = getPlayer(playerName);
    if (player != nullptr)
        player->previousEntry();
}

void listTracks(const char* playerName, PluginInterface::ListTracksCallbackFunction callback, void* userData)
{
    const Player* player = getPlayer(playerName);

    std::vector<MixerControlable*> subMixerControlables = player->getSubMixerControlables();
    for (const MixerControlable* const subMixerControlable : subMixerControlables)
    {
        callback(subMixerControlable->getName().toRawUTF8(), userData);
    }
}

void trackVolume(const char* playerName, const char* trackName, float volume)
{
    const MessageManagerLock mmLock;
    Player* player = getPlayer(playerName);
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

    DirectoryIterator directoryIterator(
        File::getSpecialLocation(File::currentExecutableFile).getSiblingFile("plugins"), false, "*.dll");
    std::vector<File> dlls;

    struct LoadFailure
    {
        String file;
        String error;
    };
    Array<LoadFailure> failedPlugins;
    while (directoryIterator.next())
    {
        Plugin plugin;

        plugin.name = directoryIterator.getFile().getFileNameWithoutExtension().toStdString();
        plugin.dynamicLibrary = std::make_unique<DynamicLibrary>(directoryIterator.getFile().getFullPathName());

        if (plugin.dynamicLibrary->getNativeHandle() == nullptr)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Unable to open the plugin."});
            continue;
        }

        auto versionFunction = reinterpret_cast<PluginInterface::MStarPluginVersion>(
            plugin.dynamicLibrary->getFunction("mstarPluginVersion"));

        if (!versionFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(),
                "Function 'mstarPluginVersion' not found. Is this a M*Player plugin?"});
            continue;
        }

        {
            int version = versionFunction();
            if (version != 1)
            {
                failedPlugins.add({directoryIterator.getFile().getFileName(),
                    "Plugin has version " + String(version) + " but M*Player requires version 1."});
                continue;
            }
        }

        plugin.initFunction
            = reinterpret_cast<PluginInterface::InitFunction>(plugin.dynamicLibrary->getFunction("init"));
        if (!plugin.initFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'init'."});
            continue;
        }

        plugin.playingStateChangedFunction = reinterpret_cast<PluginInterface::PlayingStateChangedFunction>(
            plugin.dynamicLibrary->getFunction("playingStateChanged"));
        if (!plugin.playingStateChangedFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'playingStateChanged'."});
            continue;
        }

        plugin.nextEntrySelectedFunction = reinterpret_cast<PluginInterface::NextEntrySelectedFunction>(
            plugin.dynamicLibrary->getFunction("nextEntrySelected"));
        if (!plugin.nextEntrySelectedFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'nextEntrySelected'."});
            continue;
        }

        plugin.previousEntrySelectedFunction = reinterpret_cast<PluginInterface::PreviousEntrySelectedFunction>(
            plugin.dynamicLibrary->getFunction("previousEntrySelected"));
        if (!plugin.previousEntrySelectedFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'previousEntrySelected'."});
            continue;
        }

        plugin.playlistEntrySelectedFunction = reinterpret_cast<PluginInterface::PlaylistEntrySelectedFunction>(
            plugin.dynamicLibrary->getFunction("playlistEntrySelected"));
        if (!plugin.playlistEntrySelectedFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'playlistEntrySelected'."});
            continue;
        }

        plugin.trackVolumeChangedFunction = reinterpret_cast<PluginInterface::TrackVolumeChangedFunction>(
            plugin.dynamicLibrary->getFunction("trackVolumeChanged"));
        if (!plugin.trackVolumeChangedFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'trackVolumeChanged'."});
            continue;
        }

        plugin.positionChangedFunction = reinterpret_cast<PluginInterface::PositionChangedFunction>(
            plugin.dynamicLibrary->getFunction("positionChanged"));
        if (!plugin.positionChangedFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'positionChanged'."});
            continue;
        }

        plugin.configureFunction
            = reinterpret_cast<PluginInterface::ConfigureFunction>(plugin.dynamicLibrary->getFunction("configure"));
        if (!plugin.configureFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'configure'."});
            continue;
        }

        plugin.shutdownFunction
            = reinterpret_cast<PluginInterface::ShutdownFunction>(plugin.dynamicLibrary->getFunction("shutdown"));
        if (!plugin.shutdownFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'shutdown'."});
            continue;
        }

        plugin.loadConfigurationFunction = reinterpret_cast<PluginInterface::LoadConfigurationFunction>(
            plugin.dynamicLibrary->getFunction("loadConfiguration"));
        if (!plugin.loadConfigurationFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'loadConfiguration'."});
            continue;
        }

        plugin.getConfigurationFunction = reinterpret_cast<PluginInterface::GetConfigurationFunction>(
            plugin.dynamicLibrary->getFunction("getConfiguration"));
        if (!plugin.getConfigurationFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'getConfiguration'."});
            continue;
        }

        plugin.freeConfigurationTextFunction = reinterpret_cast<PluginInterface::FreeConfigurationTextFunction>(
            plugin.dynamicLibrary->getFunction("freeConfigurationText"));
        if (!plugin.freeConfigurationTextFunction)
        {
            failedPlugins.add({directoryIterator.getFile().getFileName(), "Missing function 'freeConfigurationText'."});
            continue;
        }

        plugins.emplace_back(std::move(plugin));
    }

    PluginInterface::Init init;
    init.listPlayers = &listPlayers;
    init.listTracks = &listTracks;
    init.play = &play;
    init.stop = &stop;
    init.next = &nextEntry;
    init.previous = &previousEntry;
    init.setTrackVolume = &trackVolume;

    for (const auto& plugin : plugins)
    {
        plugin.initFunction(init);
    }

    if (failedPlugins.size() > 0)
    {
        String lines;
        for (const LoadFailure& failure : failedPlugins)
        {
            lines += "\n" + failure.file + " (" + failure.error + ")";
        }
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            TRANS("Problems while loading plugins"),
            TRANS("The following plugins failed to load:\n") + lines);
    }
}

PluginLoader::~PluginLoader()
{
    for (const auto& plugin : plugins)
    {
        plugin.shutdownFunction();
    }
}

size_t PluginLoader::count()
{
    return plugins.size();
}

String PluginLoader::pluginName(size_t index)
{
    return plugins[index].name;
}

void PluginLoader::configure(size_t index)
{
    plugins[index].configureFunction();
}

void PluginLoader::playingStateChanged(const char* playerName, bool isPlaying)
{
    for (const auto& plugin : plugins)
    {
        plugin.playingStateChangedFunction(playerName, isPlaying);
    }
}

void PluginLoader::nextEntrySelected(const char* playerName)
{
    for (const auto& plugin : plugins)
    {
        plugin.nextEntrySelectedFunction(playerName);
    }
}

void PluginLoader::previousEntrySelected(const char* playerName)
{
    for (const auto& plugin : plugins)
    {
        plugin.previousEntrySelectedFunction(playerName);
    }
}

void PluginLoader::playlistEntrySelected(const char* playerName, size_t entryIndex)
{
    for (const auto& plugin : plugins)
    {
        plugin.playlistEntrySelectedFunction(playerName, entryIndex);
    }
}

void PluginLoader::trackVolumeChanged(const char* playerName, const char* trackName, float volume)
{
    for (const auto& plugin : plugins)
    {
        plugin.trackVolumeChangedFunction(playerName, trackName, volume);
    }
}

void PluginLoader::positionChanged(const char* playerName, double position)
{
    for (const auto& plugin : plugins)
    {
        plugin.positionChangedFunction(playerName, position);
    }
}

void PluginLoader::saveConfigurations(XmlElement* pluginsElement)
{
    for (auto&& plugin : plugins)
    {
        XmlElement* pluginElement = new XmlElement("Plugin");
        pluginElement->setAttribute("name", plugin.name);
        const char* data = plugin.getConfigurationFunction();
        pluginElement->addTextElement(String::fromUTF8(data));
        plugin.freeConfigurationTextFunction(data);
        pluginsElement->addChildElement(pluginElement);
    }
}

void PluginLoader::loadConfigurations(XmlElement* pluginsElement)
{
    for (int i = 0; i < pluginsElement->getNumChildElements(); ++i)
    {
        XmlElement* pluginElement = pluginsElement->getChildElement(i);
        String configurationText = pluginElement->getAllSubText();
        std::string pluginName = pluginElement->getStringAttribute("name").toStdString();
        auto it = std::find_if(
            plugins.begin(), plugins.end(), [pluginName](const Plugin& plugin) { return plugin.name == pluginName; });
        if (it != plugins.end())
        {
            it->loadConfigurationFunction(configurationText.toRawUTF8());
        }
    }
}

PluginLoader::Plugin::Plugin() {}

PluginLoader::Plugin::Plugin(Plugin&& other)
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