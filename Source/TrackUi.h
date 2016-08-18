#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerControlable.h"
#include "Track.h"
#include "TrackEditDialog.h"
#include "VolumeSlider.h"

/**
	User interface for a track, contained in TracksComponent.
*/
class TrackUi
    : public Component
    , public ChangeListener
    , public Button::Listener
    , public juce::Slider::Listener
    , public MixerControlableChangeListener
{
public:
    typedef std::function<void(double)> SetPositionCallback;
    typedef std::function<void(Track*)> RemoveTrackCallback;
    typedef std::function<bool(int)> TrackHasFilesCallback;
    typedef std::function<void(String)> FileLoadedCallback;

    TrackUi(Track& track, ApplicationProperties& applicationProperties, SetPositionCallback setPositionCallback, RemoveTrackCallback removeTrackCallback, TrackHasFilesCallback trackHasFilesCallback, FileLoadedCallback fileLoadedCallback);
    ~TrackUi();

    void changeListenerCallback(ChangeBroadcaster* source);
    void loadFile();
    void loadFile(const File& file);
    void setLongestDuration(double duration);
    void positionChanged(double position);

    void updateIdText();

private:
    void fileChanged(const File& file, bool updatePlaylist);

    Track& m_track;

    Label m_idLabel;
    Label m_descriptionLabel;
    ImageButton m_editButton;
    ImageButton m_soloButton;
    ImageButton m_muteButton;
    VolumeSlider m_volumeSlider;
    Label m_fileNameLabel;

    ScopedPointer<TrackEditDialogWindow> m_editDialog;

    double m_longestDuration;
    double m_progress; // the progress of the playback

    ApplicationProperties& m_applicationProperties;

    SetPositionCallback m_setPositionCallback;

    RemoveTrackCallback m_removeTrackCallback;

    TrackHasFilesCallback m_trackHasFilesCallback;

    FileLoadedCallback m_fileLoadedCallback;

// Button::Listener
public:
    void buttonClicked(Button* button);

// Component
public:
    virtual void paint(Graphics&) override;
    virtual void resized() override;
    virtual void mouseDown(const MouseEvent& event) override;
    virtual void mouseDrag(const MouseEvent& event) override;

// juce::Slider::Listener
public:
    virtual void sliderValueChanged(Slider* slider) override;

// MixerControlableChangeListener
public:
    virtual void gainChanged(float gain) override;
    virtual void muteChanged(bool /*mute*/) override;
    virtual void soloChanged(bool /*solo*/) override;
    virtual void nameChanged(const String& name) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackUi)
};