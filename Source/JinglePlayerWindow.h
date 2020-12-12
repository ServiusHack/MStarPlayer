#pragma once

#include "ChannelMappingDialog.h"
#include "InterPlayerCommunication.h"
#include "MixerComponent.h"
#include "OutputChannelNames.h"
#include "PlayerEditDialog.h"
#include "TracksContainer.h"
#include "Utils.h"
#include "Waveform.h"

class Player;

/**
        User interface with a single big button to control the playback.
*/
class JinglePlayerWindow
    : public juce::Component
    , public juce::Button::Listener
    , public juce::ChangeListener
    , public juce::FileDragAndDropTarget
{
public:
    /** Creates a new JinglePlayer.

        @param mixer              Mix the jingle audio into this mixer.

        @param outputChannels     Number of output channels when the JinglePlayer is created.
                                  When this changes later the setOutputChannels method is called.
    */
    JinglePlayerWindow(Player& player, TracksContainer* tracksContainer,
        InterPlayerCommunication::ShowEditDialogCallback showEditDialogCallback,
        InterPlayerCommunication::ConfigureChannelsCallback configureChannelsCallback,
        InterPlayerCommunication::ConfigureMidiCallback configureMidiCallback,
        InterPlayerCommunication::ChangePlayerTypeCallback changePlayerTypeCallback,
        InterPlayerCommunication::SetUserImageCallback setUserImageCallback);

    void setColor(const juce::Colour& color);

    void setUserImage(const juce::File& file);

    // Component overrides
public:
    virtual void resized() override;
    virtual void paint(juce::Graphics& g) override;
    /** Show the configuration menu. */
    virtual void mouseDown(const juce::MouseEvent& event) override;

    // Button::Listener
public:
    /** Play or stop the audio playback. */
    virtual void buttonClicked(juce::Button* /*button*/) override;

    // ChangeListener
public:
    virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // FileDragAndDropTarget
public:
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    // ui values
    double m_progress; // the progress of the playback
    double m_totalLength;

    // ui components
    juce::ProgressBar m_progressBar;
    juce::Label m_totalDurationText;
    juce::DrawableButton m_playButton;
    juce::ImageButton m_configureButton;
    std::unique_ptr<juce::Drawable> m_playImage;
    std::unique_ptr<juce::Drawable> m_stopImage;
    std::unique_ptr<juce::Drawable> m_userImage;
    juce::Label m_fileNameLabel;

    // configuration menu actions
    void loadFile();

    // audio output
    bool m_showRemainingTime;

    juce::Colour m_color;
    bool m_blink;
    juce::Colour m_paintColor;
    void updatePointColor();

    Player& m_player;

    TracksContainer* m_tracksContainer;

    InterPlayerCommunication::ShowEditDialogCallback m_showEditDialogCallback;
    InterPlayerCommunication::ConfigureChannelsCallback m_configureChannelsCallback;
    InterPlayerCommunication::ConfigureMidiCallback m_configureMidiCallback;
    InterPlayerCommunication::ChangePlayerTypeCallback m_changePlayerTypeCallback;
    InterPlayerCommunication::SetUserImageCallback m_setUserImageCallback;

    juce::AudioFormatManager m_formatManager;

    Waveform m_waveform;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JinglePlayerWindow)
};
