#include "TrackEditDialog.h"

TrackEditDialogWindow::TrackEditDialogWindow(juce::String name, float trackGain,
    TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback)
    : juce::DialogWindow(TRANS("Edit track"), juce::Colours::lightgrey, true, true)
{
    TrackEditDialogComponent* component
        = new TrackEditDialogComponent(name, trackGain, settingsChangedCallback, volumeChangedCallback, this);
    setContentOwned(component, true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(false, false);
}

void TrackEditDialogWindow::closeButtonPressed()
{
    setVisible(false);
}

bool TrackEditDialogWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::returnKey)
    {
        exitModalState(0);
        return true;
    }

    return false;
}

void TrackEditDialogWindow::focusGained(juce::DialogWindow::FocusChangeType /*cause*/)
{
    static_cast<TrackEditDialogComponent*>(getContentComponent())->m_nameEditor.grabKeyboardFocus();
}

void TrackEditDialogWindow::buttonClicked(juce::Button* /*buttonThatWasClicked*/)
{
    closeButtonPressed();
}

TrackEditDialogComponent::TrackEditDialogComponent(juce::String name, float trackGain,
    TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback,
    TrackEditDialogWindow* parent)
    : m_settingsChangedCallback(settingsChangedCallback)
    , m_volumeChangedCallback(volumeChangedCallback)
    , m_nameLabel("name label", TRANS("Name of the track:"))
    , m_nameEditor("name editor")
    , m_closeButton("close")
{
    addAndMakeVisible(m_nameLabel);
    m_nameLabel.setFont(juce::Font(15.00f, juce::Font::plain));
    m_nameLabel.setJustificationType(juce::Justification::centredLeft);
    m_nameLabel.setEditable(false, false, false);
    m_nameLabel.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_nameLabel.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    addAndMakeVisible(m_nameEditor);
    m_nameEditor.setMultiLine(false);
    m_nameEditor.setReturnKeyStartsNewLine(false);
    m_nameEditor.setReadOnly(false);
    m_nameEditor.setScrollbarsShown(true);
    m_nameEditor.setCaretVisible(true);
    m_nameEditor.setPopupMenuEnabled(true);
    m_nameEditor.setEscapeAndReturnKeysConsumed(false);
    m_nameEditor.setText(name);
    m_nameEditor.selectAll();
    m_nameEditor.addListener(this);

    addAndMakeVisible(m_volumeSlider);
    m_volumeSlider.setValue(trackGain);
    m_volumeSlider.addListener(this);

    addAndMakeVisible(m_closeButton);
    m_closeButton.setButtonText(TRANS("Close"));
    m_closeButton.addListener(parent);
    m_closeButton.setWantsKeyboardFocus(false);

    setWantsKeyboardFocus(false);

    setSize(250, 200);
}

void TrackEditDialogComponent::resized()
{
    const static int buttonWidth = 80;
    const static int rowHeight = 24;
    const static int padding = 10;
    const static int volumeSliderWidth = 16;

    m_volumeSlider.setBounds(padding, padding, volumeSliderWidth, getHeight() - rowHeight - 3 * padding);

    m_nameLabel.setBounds(
        padding + volumeSliderWidth + padding, padding, getWidth() - 3 * padding - volumeSliderWidth, rowHeight);
    m_nameEditor.setBounds(padding + volumeSliderWidth + padding,
        padding + rowHeight,
        getWidth() - 3 * padding - volumeSliderWidth,
        rowHeight);

    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - 2 * (rowHeight - padding), buttonWidth, rowHeight);
}

void TrackEditDialogComponent::sliderValueChanged(juce::Slider* /*slider*/)
{
    m_volumeChangedCallback(static_cast<float>(m_volumeSlider.getValue()));
}

void TrackEditDialogComponent::textEditorTextChanged(juce::TextEditor&)
{
    m_settingsChangedCallback(m_nameEditor.getText());
}
