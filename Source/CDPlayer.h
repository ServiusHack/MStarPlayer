#pragma once

#include <optional>

#include "MixerComponent.h"
#include "MixerControlable.h"

#include "CDNamesComboBox.h"
#include "CDTracksModel.h"
#include "CDTracksTable.h"
#include "MyMultiDocumentPanel.h"
#include "SoloBusSettings.h"
#include "SubchannelPlayer.h"

#include "ChannelMappingDialog.h"
#include "PlayerComponent.h"
#include "PlayerEditDialog.h"
#include "PluginLoader.h"

/**
    A player for audio CDs
*/
class CDPlayer
    : public PlayerComponent
    , public juce::KeyListener
    , public juce::Button::Listener
    , public juce::ComboBox::Listener
    , public juce::Slider::Listener
    , public SoloBusSettingsListener
    , private juce::Timer
{
public:
    CDPlayer(MixerComponent* mixer, OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings,
        juce::TimeSliceThread& thread, PluginLoader& pluginLoader, float gain = 1.0f, bool solo = false,
        bool mute = false);
    ~CDPlayer();

    void play() override;
    void pause() override;
    void stop() override;
    void nextEntry(bool onlyIfEntrySaysSo = false) override;
    void previousEntry() override;

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
    virtual void paint(juce::Graphics&) override;
    virtual void resized() override;
    virtual void mouseDown(const juce::MouseEvent& event) override;

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

    // Button::Listener
public:
    virtual void buttonClicked(juce::Button* /*button*/) override;

    // ComboBox::Listener
public:
    virtual void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    // Slider::Listener
public:
    void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;

    // SoloBusListener
public:
    void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) override;

    // Timer
private:
    void timerCallback() override;

private:
    void updateGain();

    void setColor(const juce::Colour& color);

    void showEditDialog();

    std::vector<std::pair<char, int>> createMapping();
    void configureChannels();

    void setNextReadPosition(juce::int64 sampleInCDSampleRate);

    MixerComponent* m_mixer;
    OutputChannelNames* m_outputChannelNames;
    SoloBusSettings& m_soloBusSettings;

    float m_gain;
    bool m_soloMute;
    bool m_solo;
    bool m_mute;

    juce::Colour m_color;

    PluginLoader& m_pluginLoader;
    juce::TimeSliceThread& m_thread;

    juce::ImageButton m_playButton;
    juce::ImageButton m_pauseButton;
    juce::ImageButton m_stopButton;
    juce::ImageButton m_skipBackwardButton;
    juce::ImageButton m_skipForwardButton;
#if JUCE_WINDOWS
    juce::ImageButton m_ejectButton;
#endif
    juce::ImageButton m_configureButton;
    juce::Label m_digitalDisplay;
    CDNamesComboBox m_availableCDsComboBox;
    juce::Slider m_slider;
    std::unique_ptr<juce::AudioCDReader> m_reader;
    std::unique_ptr<juce::AudioFormatReaderSource> m_source;
    std::optional<CDTracksModel> m_model;
    CDTracksTable m_tracksTable;
    juce::AudioTransportSource m_transportSource;
    ChannelRemappingAudioSourceWithVolume m_remappingAudioSource;

    std::optional<PlayerEditDialogWindow> m_PlayerEditDialog;
    std::optional<ChannelMappingWindow> m_channelMappingWindow;

    int m_currentTrack = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CDPlayer)
};
