#include "PlaylistEntryDialog.h"

PlaylistEntryDialogWindow::PlaylistEntryDialogWindow(
    juce::String name, PlaylistEntrySettingsChangedCallback changedCallback)
    : DialogWindow(TRANS("Edit playlist entry"), juce::Colours::lightgrey, true, true)
{
    PlaylistEntryDialogComponent* component = new PlaylistEntryDialogComponent(name, changedCallback, this);
    setContentOwned(component, true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(false, false);
}

void PlaylistEntryDialogWindow::closeButtonPressed()
{
    setVisible(false);
}

bool PlaylistEntryDialogWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::returnKey)
    {
        exitModalState(0);
        return true;
    }

    return false;
}

void PlaylistEntryDialogWindow::focusGained(juce::DialogWindow::FocusChangeType /*cause*/)
{
    static_cast<PlaylistEntryDialogComponent*>(getContentComponent())->m_nameEditor.grabKeyboardFocus();
}

void PlaylistEntryDialogWindow::buttonClicked(juce::Button* /*buttonThatWasClicked*/)
{
    closeButtonPressed();
}

PlaylistEntryDialogComponent::PlaylistEntryDialogComponent(const juce::String& name,
    const PlaylistEntrySettingsChangedCallback& changedCallback, PlaylistEntryDialogWindow* parent)
    : m_changedCallback(changedCallback)
    , m_nameLabel("name label", TRANS("Name of the entry:"))
    , m_nameEditor("name editor")
    , m_closeButton("close")
{
    addAndMakeVisible(m_nameLabel);
    m_nameLabel.setFont(juce::Font(juce::FontOptions(15.00f, juce::Font::plain)));
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

    addAndMakeVisible(m_closeButton);
    m_closeButton.setButtonText(TRANS("Close"));
    m_closeButton.addListener(parent);
    m_closeButton.setWantsKeyboardFocus(false);

    setWantsKeyboardFocus(false);

    setSize(250, 200);
}

void PlaylistEntryDialogComponent::resized()
{
    const static int buttonWidth = 80;
    const static int rowHeight = 24;
    const static int padding = 10;

    m_nameLabel.setBounds(padding, padding, getWidth() - 2 * padding, rowHeight);
    m_nameEditor.setBounds(padding, padding + rowHeight, getWidth() - 2 * padding, rowHeight);

    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - 2 * (rowHeight - padding), buttonWidth, rowHeight);
}

void PlaylistEntryDialogComponent::textEditorTextChanged(juce::TextEditor&)
{
    m_changedCallback(m_nameEditor.getText());
}
