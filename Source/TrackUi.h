#pragma once

#include <optional>

#include "juce_gui_basics/juce_gui_basics.h"

#include "MixerControlable.h"
#include "Track.h"
#include "TrackEditDialog.h"
#include "VolumeSlider.h"
#include "Waveform.h"

/**
        User interface for a track, contained in TracksComponent.
*/
class TrackUi
    : public juce::Component
    , public juce::ChangeListener
    , public juce::Button::Listener
    , public juce::Slider::Listener
    , public MixerControlableChangeListener
{
public:
    using SetPositionCallback = std::function<void(double)>;
    using RemoveTrackCallback = std::function<void(Track*)>;
    using TrackHasFilesCallback = std::function<bool(int)>;
    using FileLoadedCallback = std::function<void(juce::String)>;

    TrackUi(Track& track, juce::ApplicationProperties& applicationProperties, SetPositionCallback setPositionCallback,
        RemoveTrackCallback removeTrackCallback, TrackHasFilesCallback trackHasFilesCallback,
        FileLoadedCallback fileLoadedCallback);
    ~TrackUi();

    void changeListenerCallback(juce::ChangeBroadcaster* source);
    void loadFile();
    void loadFile(const juce::File& file);
    void setLongestDuration(double duration);
    void positionChanged(double position);

    void updateIdText();

private:
    void fileChanged(const juce::File& file, bool updatePlaylist);

    Track& m_track;

    juce::Label m_idLabel;
    juce::Label m_descriptionLabel;
    juce::ImageButton m_editButton;
    juce::ImageButton m_soloButton;
    juce::ImageButton m_muteButton;
    VolumeSlider m_volumeSlider;
    juce::Label m_fileNameLabel;

    std::unique_ptr<TrackEditDialogWindow> m_editDialog;

    double m_longestDuration;
    double m_progress; // the progress of the playback

    juce::ApplicationProperties& m_applicationProperties;

    SetPositionCallback m_setPositionCallback;

    RemoveTrackCallback m_removeTrackCallback;

    TrackHasFilesCallback m_trackHasFilesCallback;

    FileLoadedCallback m_fileLoadedCallback;

    Waveform m_waveform;

    std::optional<juce::FileChooser> m_currentFileChooser;

    // Button::Listener
public:
    void buttonClicked(juce::Button* button);

    // Component
public:
    virtual void paint(juce::Graphics&) override;
    virtual void paintOverChildren(juce::Graphics&) override;
    virtual void resized() override;
    virtual void mouseDown(const juce::MouseEvent& event) override;
    virtual void mouseDrag(const juce::MouseEvent& event) override;

    // juce::Slider::Listener
public:
    virtual void sliderValueChanged(juce::Slider* slider) override;

    // MixerControlableChangeListener
public:
    virtual void gainChanged(float gain) override;
    virtual void muteChanged(bool /*mute*/) override;
    virtual void soloChanged(bool /*solo*/) override;
    virtual void nameChanged(const juce::String& name) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackUi)
};
