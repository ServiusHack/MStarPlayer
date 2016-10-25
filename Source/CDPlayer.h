#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerComponent.h"
#include "MixerControlable.h"

#include "CDNamesComboBox.h"
#include "CDTracksModel.h"
#include "CDTracksTable.h"
#include "SoloBusSettings.h"
#include "SubchannelPlayer.h"
#include "MyMultiDocumentPanel.h"

#include "ChannelMappingDialog.h"
#include "PlayerEditDialog.h"

/**
    A player for audio CDs
*/
class CDPlayer 
    : public Component
    , public SubchannelPlayer
    , public KeyListener
    , public Button::Listener
    , public ComboBox::Listener
    , public Slider::Listener
    , public SoloBusSettingsListener
    , private Timer
{
public:
    CDPlayer(MixerComponent* mixer, OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings, TimeSliceThread& thread, float gain = 1.0f, bool solo = false, bool mute = false);
    ~CDPlayer();

// XML serialization
public:
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    XmlElement* saveToXml(const File& projectDirectory, MyMultiDocumentPanel::LayoutMode layoutMode) const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml(const XmlElement& element, const File& projectDirectory);

// Component overrides
public:
    virtual void paint(Graphics&) override;
    virtual void resized() override;
    virtual void mouseDown(const MouseEvent& event) override;

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
    virtual String getName() const override;
    virtual void setName(const String& newName) override;
    virtual void SetChannelCountChangedCallback(const Track::ChannelCountChangedCallback& callback) override;
    virtual std::vector<MixerControlable*> getSubMixerControlables() const override;

// KeyListener
public:
    virtual bool keyPressed(const KeyPress& key, Component* originatingComponent) override;

// Button::Listener
public:
    virtual void buttonClicked(Button* /*button*/) override;

// ComboBox::Listener
public:
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

// Slider::Listener
public:
    void sliderValueChanged(Slider* sliderThatWasMoved) override;

// SoloBusListener
public:
    void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) override;

// Timer
private:
    void timerCallback() override;

private:
    void updateGain();

    void setColor(const Colour& color);

    void showEditDialog();

    std::vector<std::pair<char, int>> createMapping();
    void configureChannels();

    void setNextReadPosition(int64 sampleInCDSampleRate);

    MixerComponent* m_mixer;
    OutputChannelNames* m_outputChannelNames;
    SoloBusSettings& m_soloBusSettings;

    float m_gain;
    bool m_soloMute;
    bool m_solo;
    bool m_mute;

    Colour m_color;

    TimeSliceThread& m_thread;

    ImageButton m_playButton;
    ImageButton m_pauseButton;
    ImageButton m_stopButton;
    ImageButton m_skipBackwardButton;
    ImageButton m_skipForwardButton;
    ImageButton m_ejectButton;
    ImageButton m_configureButton;
    Label m_digitalDisplay;
    CDNamesComboBox m_availableCDsComboBox;
    Slider m_slider;
    OptionalScopedPointer<AudioCDReader> m_reader;
    OptionalScopedPointer<AudioFormatReaderSource> m_source;
    OptionalScopedPointer<CDTracksModel> m_model;
    CDTracksTable m_tracksTable;
    AudioTransportSource m_transportSource;
    ChannelRemappingAudioSourceWithVolume m_remappingAudioSource;

    OptionalScopedPointer<PlayerEditDialogWindow> m_PlayerEditDialog;
    OptionalScopedPointer<ChannelMappingWindow> m_channelMappingWindow;

    int m_currentTrack = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CDPlayer)
};
