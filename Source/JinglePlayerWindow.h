#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ChannelMappingDialog.h"
#include "InterPlayerCommunication.h"
#include "MixerComponent.h"
#include "OutputChannelNames.h"
#include "PlayerEditDialog.h"
#include "TracksContainer.h"
#include "Utils.h"

/**
	User interface with a single big button to control the playback.
*/
class JinglePlayerWindow
    : public Component
    , public Button::Listener
    , public ChangeListener
    , public FileDragAndDropTarget
{
public:
    /** Creates a new JinglePlayer.
    
        @param mixer              Mix the jingle audio into this mixer.

        @param outputChannels     Number of output channels when the JinglePlayer is created.
                                  When this changes later the setOutputChannels method is called.
    */
    JinglePlayerWindow(TracksContainer* tracksContainer, InterPlayerCommunication::ShowEditDialogCallback showEditDialogCallback, InterPlayerCommunication::ConfigureChannelsCallback configureChannelsCallback, InterPlayerCommunication::ChangePlayerTypeCallback changePlayerTypeCallback, InterPlayerCommunication::SetUserImageCallback setUserImageCallback);

    void setColor(const Colour& color);

    void setUserImage(const File& file);

// Component overrides
public:
    virtual void resized() override;
    virtual void paint(Graphics& g) override;
    /** Show the configuration menu. */
    virtual void mouseDown(const MouseEvent& event) override;

// Button::Listener
public:
    /** Play or stop the audio playback. */
    virtual void buttonClicked(Button* /*button*/) override;

// ChangeListener
public:
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;

// FileDragAndDropTarget
public:
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

private:
    // ui values
    double m_progress; // the progress of the playback
    double m_totalLength;

    // ui components
    ScopedPointer<ProgressBar> m_progressBar;
    ScopedPointer<Label> m_totalDurationText;
    ScopedPointer<DrawableButton> m_playButton;
    ScopedPointer<ImageButton> m_configureButton;
    ScopedPointer<Drawable> m_playImage;
    ScopedPointer<Drawable> m_stopImage;
    OptionalScopedPointer<Drawable> m_userImage;
    ScopedPointer<Label> m_fileNameLabel;

    // configuration menu actions
    void loadFile();

    // audio output
    OptionalScopedPointer<ChannelMappingWindow> m_channelMappingWindow;

    OptionalScopedPointer<PlayerEditDialogWindow> m_PlayerEditDialog;

    bool m_showRemainingTime;

    Colour m_color;
    bool m_blink;
    Colour m_paintColor;
    void updatePointColor();

    TracksContainer* m_tracksContainer;

    InterPlayerCommunication::ShowEditDialogCallback m_showEditDialogCallback;
    InterPlayerCommunication::ConfigureChannelsCallback m_configureChannelsCallback;
    InterPlayerCommunication::ChangePlayerTypeCallback m_changePlayerTypeCallback;
    InterPlayerCommunication::SetUserImageCallback m_setUserImageCallback;

    AudioFormatManager m_formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JinglePlayerWindow)
};
