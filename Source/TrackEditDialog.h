#pragma once

#include <functional>

#include "juce_gui_basics/juce_gui_basics.h"

#include "VolumeSlider.h"

typedef std::function<void(juce::String)> TrackSettingsChangedCallback;
typedef std::function<void(float)> VolumeChangedCallback;

/**
    Window wrapper for the TrackEditDialogComponent.

    This allows the component to be shown in its own window.
*/
class TrackEditDialogWindow
    : public juce::DialogWindow
    , public juce::Button::Listener
{
public:
    TrackEditDialogWindow(juce::String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback,
        VolumeChangedCallback volumeChangedCallback);

    // DialogWindow overrides
public:
    virtual void closeButtonPressed() override;
    virtual bool keyPressed(const juce::KeyPress& key) override;
    virtual void focusGained(juce::DialogWindow::FocusChangeType cause) override;

    // Button::Listener
public:
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditDialogWindow)
};

/**
        The actual component containing controls to edit a track.
*/
class TrackEditDialogComponent
    : public juce::Component
    , public juce::Slider::Listener
    , public juce::TextEditor::Listener
{
    friend class TrackEditDialogWindow;

public:
    TrackEditDialogComponent(juce::String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback,
        VolumeChangedCallback volumeChangedCallback, TrackEditDialogWindow* parent);

    // Component overrides
public:
    virtual void resized() override;

    // Slider::Listener
public:
    virtual void sliderValueChanged(juce::Slider* slider) override;

    // TextEditor::Listener
public:
    virtual void textEditorTextChanged(juce::TextEditor&) override;

private:
    juce::Label m_nameLabel;
    juce::TextEditor m_nameEditor;
    juce::TextButton m_closeButton;
    VolumeSlider m_volumeSlider;

    TrackSettingsChangedCallback m_settingsChangedCallback;
    VolumeChangedCallback m_volumeChangedCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditDialogComponent)
};
