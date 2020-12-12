#pragma once

#include <set>

#include "juce_gui_basics/juce_gui_basics.h"

#include "MixerComponent.h"
#include "MixerControlable.h"

#include "InterPlayerCommunication.h"
#include "JinglePlayerWindow.h"
#include "MyMultiDocumentPanel.h"
#include "PlayerComponent.h"
#include "PlayerMidiDialog.h"
#include "PlaylistModel.h"
#include "PlaylistPlayerWindow.h"
#include "PluginLoader.h"
#include "SubchannelPlayer.h"
/**
        A player with a playlist and tracks to play audio files.

        The different types of players only differ in their user interface. The actual logic
        is almost the same.
*/
class Player
    : public PlayerComponent
    , public juce::KeyListener
    , public SoloBusSettingsListener
{
public:
    Player(MixerComponent* mixer, OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings,
        InterPlayerCommunication::PlayerType type, juce::ApplicationProperties& applicationProperties,
        juce::AudioThumbnailCache& audioThumbnailCache, juce::TimeSliceThread& thread, MTCSender& mtcSender,
        PluginLoader& pluginLoader, float gain = 1.0f, bool solo = false, bool mute = false);
    ~Player();

    void setType(InterPlayerCommunication::PlayerType type);
    void play() override;
    void pause() override;
    void stop() override;
    void nextEntry(bool onlyIfEntrySaysSo = false) override;
    void previousEntry() override;
    void playlistEntryChanged(const std::vector<TrackConfig>& trackConfigs, bool play, int index);
    void gainChangedCallback(const char* track_name, float gain);

    // XML serialization
public:
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    juce::XmlElement* saveToXml(const juce::File& projectDirectory, MyMultiDocumentPanel::LayoutMode layoutMode) const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml(const juce::XmlElement& element, const juce::File& projectDirectory);

    // Component overrides
public:
    virtual void resized() override;

    // SubchannelPlayer
public:
    virtual void setGain(float gain) override;
    virtual float getGain() const override;
    virtual void setPan(float pan) override;
    virtual float getPan() const override;
    virtual void setSoloMute(bool soloMute) override;
    virtual bool getSoloMute() const override;
    virtual void setSolo(bool solo) override;
    virtual bool getSolo() const override;
    virtual void setMute(bool mute) override;
    virtual bool getMute() const override;
    virtual float getVolume() const override;
    virtual juce::String getName() const override;
    virtual void setName(const juce::String& newName) override;
    virtual void SetChannelCountChangedCallback(const Track::ChannelCountChangedCallback& callback) override;
    virtual std::vector<MixerControlable*> getSubMixerControlables() const override;

    // KeyListener
public:
    virtual bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

    // SoloBusListener
public:
    void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) override;

public:
    /** Set the number of output channels.

        If the user reconfigures his audio settings the number of output channels
        might change. This method is called to propagate this change to this player.
    */
    void setOutputChannels(int outputChannels);

private:
    void updateGain();

    void setColor(const juce::Colour& color);
    void setUserImage(const juce::File& file);

    void showEditDialog();
    void configureChannels();
    void configureMidi();

    void trackConfigChanged();

    MixerComponent* m_mixer;
    OutputChannelNames* m_outputChannelNames;
    SoloBusSettings& m_soloBusSettings;

    PlaylistModel playlistModel;
    int currentPlaylistEntry{-1};
    TracksContainer m_tracksContainer;

    PluginLoader& m_pluginLoader;

    PlaylistPlayerWindow m_playlistPlayer;
    JinglePlayerWindow m_jinglePlayer;

    float m_gain;
    bool m_soloMute;
    bool m_solo;
    bool m_mute;
    InterPlayerCommunication::PlayerType m_type;

    juce::Colour m_color;
    juce::File m_userImage;

    Track::ChannelCountChangedCallback m_channelCountChanged;
    std::unique_ptr<PlayerEditDialogWindow> m_PlayerEditDialog;
    std::unique_ptr<ChannelMappingWindow> m_channelMappingWindow;
    std::unique_ptr<PlayerMidiDialogWindow> m_PlayerMidiDialog;

    MTCSender& m_mtcSender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Player)
};
