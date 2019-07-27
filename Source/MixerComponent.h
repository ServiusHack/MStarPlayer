#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ChannelVolumeAudioSource.h"
#include "MixerFader.h"
#include "OutputChannelNames.h"
#include "PlayerMixerFader.h"
#include "SubchannelPlayer.h"

#include <memory>

/** Shows sliders for output channels to change the individual volumes of them.

        This component is shown at the bottom and allows the user to change the volume
        of each output channel individually. The actual work is done by
        ChannelVolumeAudioSource, which is managed by this class.

        The audio flow looks like this:
        Players -> MixerAudioSource -> ChannelVolumeAudioSource -> AudioSourcePlayer -> AudioDeviceManager
*/
class MixerComponent
    : public Component
    , public ChangeListener
    , public OutputChannelNamesListener
    , public SoloBusSettingsListener
    , public ScrollBar::Listener
    , public Timer
{
public:
    /** Creates a new MixerComponent.

        @param audioDeviceManager The AudioDeviceManager used throughout the application.
    */
    MixerComponent(AudioDeviceManager* audioDeviceManager, OutputChannelNames* outputChannelNames,
        SoloBusSettings& soloBusSettings);
    ~MixerComponent();

    /** Returns the MixerAudioSource into which all Players mix their audio stream. */
    MixerAudioSource& getMixerAudioSource();
    ChannelVolumeAudioSource& getChannelVolumeAudioSource();

    void updatePlayerColor(SubchannelPlayer* player, Colour color);

    // Player registration
public:
    void registerPlayer(SubchannelPlayer* player);
    void unregisterPlayer(SubchannelPlayer* player);

    // XML Serialization
public:
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    void saveToXml(XmlElement* element) const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml(const XmlElement& element);

    // Slider for channels and players
private:
    /** Adds a new slider to the component because not enough are being shown. */
    void addChannelSlider();

    void addPlayerSlider(SubchannelPlayer* player);

    std::vector<std::unique_ptr<MixerFader>> m_channelSliders;
    std::vector<std::unique_ptr<PlayerMixerFader>> m_playerSliders;
    Component m_slidersContainer;
    ScrollBar m_sliderScrollBar;

    // Component
public:
    virtual void resized() override;

    // ChangeListener
public:
    /** Act accordingly to changes in the AudioDeviceManager. */
    virtual void changeListenerCallback(ChangeBroadcaster* /*source*/) override;

    // OutputChannelNamesListener
public:
    virtual void outputChannelNamesReset() override;
    virtual void outputChannelNameChanged(int activeChannelIndex, const String& text) override;
    virtual void outputChannelPairingModeChanged(int activeChannelIndex, PairingMode mode) override;

    // SoloBusSettingsListener
public:
    virtual void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) override;

    // ScrollBar::Listener
public:
    virtual void scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;

    // Timer
public:
    virtual void timerCallback() override;

private:
    // audio output
    MixerAudioSource m_mixerAudioSource;
    ChannelVolumeAudioSource m_channelVolumeAudioSource;
    AudioSourcePlayer m_audioSourcePlayer;
    AudioDeviceManager* m_audioDeviceManager;
    OutputChannelNames* m_outputChannelNames;
    SoloBusSettings& m_soloBusSettings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerComponent)
};
