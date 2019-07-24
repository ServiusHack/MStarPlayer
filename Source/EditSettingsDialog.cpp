#include "../JuceLibraryCode/JuceHeader.h"

#include "EditSettingsDialog.h"

EditSettingsWindow::EditSettingsWindow(
    ApplicationProperties& applicationProperties, const std::function<void()>& snapToGridChanged)
    : DialogWindow(TRANS("Edit Settings"), Colours::lightgrey, true, true)
{
    setContentOwned(new EditSettingsComponent(this, applicationProperties, snapToGridChanged), true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(true, true);
}

void EditSettingsWindow::closeButtonPressed()
{
    setVisible(false);
}

void EditSettingsWindow::buttonClicked(Button*)
{
    closeButtonPressed();
}

EditSettingsComponent::EditSettingsComponent(EditSettingsWindow* parent, ApplicationProperties& applicationProperties,
    const std::function<void()>& snapToGridChanged)
    : m_applicationProperties(applicationProperties)
    , m_languageLabel("language", TRANS("Language"))
    , m_nameLabel("audio editor label", TRANS("Audio editor"))
    , m_audioEditorFilenameComponent("audio editor",
          File(m_applicationProperties.getUserSettings()->getValue("audioEditor")), true, false, false, "*.exe", ".exe",
          "")
    , m_closeButton("close")
    , m_snapToGridButton(TRANS("snap players to grid"))
    , m_snapToGridWidthLabel("grid width label", TRANS("grid width:"))
    , m_snapToGridHeightLabel("grid height label", TRANS("grid height:"))
    , m_snapToGridChanged(snapToGridChanged)
{
    addAndMakeVisible(m_nameLabel);
    m_nameLabel.setFont(Font(15.00f, Font::plain));
    m_nameLabel.setJustificationType(Justification::centredLeft);
    m_nameLabel.setEditable(false, false, false);
    m_nameLabel.setColour(TextEditor::textColourId, Colours::black);
    m_nameLabel.setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    addAndMakeVisible(m_audioEditorFilenameComponent);
    m_audioEditorFilenameComponent.setDefaultBrowseTarget(File::getSpecialLocation(File::globalApplicationsDirectory));
    m_audioEditorFilenameComponent.addListener(this);

    m_languageLabel.setFont(Font(15.00f, Font::plain));
    m_languageLabel.setJustificationType(Justification::centredLeft);
    m_languageLabel.setEditable(false, false, false);
    m_languageLabel.setColour(TextEditor::textColourId, Colours::black);
    m_languageLabel.setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    addAndMakeVisible(m_languageLabel);

    m_languageComboBox.addItem("English", 1);
    m_languageComboBox.addItem("German", 2);
    if (m_applicationProperties.getUserSettings()->getValue("language") == "en")
        m_languageComboBox.setSelectedId(1, juce::dontSendNotification);
    else if (m_applicationProperties.getUserSettings()->getValue("language") == "de")
        m_languageComboBox.setSelectedId(2, juce::dontSendNotification);

    addAndMakeVisible(m_languageComboBox);
    m_languageComboBox.addListener(this);

    addAndMakeVisible(m_snapToGridButton);
    m_snapToGridButton.addListener(this);
    m_snapToGridButton.setToggleState(m_applicationProperties.getUserSettings()->getBoolValue("snapToGrid", false),
        NotificationType::sendNotification);

    addAndMakeVisible(m_snapToGridWidthLabel);
    m_snapToGridWidthLabel.setJustificationType(Justification::right);
    addAndMakeVisible(m_snapToGridWidthEditor);
    m_snapToGridWidthEditor.setInputRestrictions(3, "0123456789");
    m_snapToGridWidthEditor.addListener(this);
    m_snapToGridWidthEditor.setText(
        String(m_applicationProperties.getUserSettings()->getIntValue("snapToGridWidth", 1)));

    addAndMakeVisible(m_snapToGridHeightLabel);
    m_snapToGridHeightLabel.setJustificationType(Justification::right);
    addAndMakeVisible(m_snapToGridHeightEditor);
    m_snapToGridHeightEditor.setInputRestrictions(3, "0123456789");
    m_snapToGridHeightEditor.setText("10");
    m_snapToGridHeightEditor.addListener(this);
    m_snapToGridHeightEditor.setText(
        String(m_applicationProperties.getUserSettings()->getIntValue("snapToGridHeight", 1)));

    addAndMakeVisible(m_closeButton);
    m_closeButton.setButtonText(TRANS("Close"));
    m_closeButton.addListener(parent);
    m_closeButton.setWantsKeyboardFocus(false);

    setSize(500, 200);
}

void EditSettingsComponent::resized()
{
    const static int buttonWidth = 80;
    const static int rowHeight = 24;
    const static int padding = 10;

    m_nameLabel.setBounds(padding, padding, getWidth() - 2 * padding, rowHeight);
    m_audioEditorFilenameComponent.setBounds(padding, padding + rowHeight, getWidth() - 2 * padding, rowHeight);

    m_languageLabel.setBounds(padding, padding + rowHeight * 2, getWidth() - 2 * padding, rowHeight);
    m_languageComboBox.setBounds(padding, padding + rowHeight * 3, getWidth() - 2 * padding, rowHeight);

    m_snapToGridButton.setBounds(padding, padding + rowHeight * 4, getWidth() - 2 * padding, rowHeight);

    m_snapToGridWidthLabel.setBounds(padding, padding + rowHeight * 5, 100, rowHeight);
    m_snapToGridWidthEditor.setBounds(m_snapToGridWidthLabel.getX() + m_snapToGridWidthLabel.getWidth() + padding,
        padding + rowHeight * 5,
        60,
        rowHeight);

    m_snapToGridHeightLabel.setBounds(m_snapToGridWidthEditor.getX() + m_snapToGridWidthEditor.getWidth() + padding,
        padding + rowHeight * 5,
        100,
        rowHeight);
    m_snapToGridHeightEditor.setBounds(m_snapToGridHeightLabel.getX() + m_snapToGridHeightLabel.getWidth() + padding,
        padding + rowHeight * 5,
        60,
        rowHeight);

    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - 2 * (rowHeight - padding), buttonWidth, rowHeight);
}

void EditSettingsComponent::filenameComponentChanged(FilenameComponent* /*fileComponentThatHasChanged*/)
{
    m_applicationProperties.getUserSettings()->setValue(
        "audioEditor", m_audioEditorFilenameComponent.getCurrentFileText());
}

void EditSettingsComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    switch (comboBoxThatHasChanged->getSelectedId())
    {
    case 1:
        m_applicationProperties.getUserSettings()->setValue("language", "en");
        break;
    case 2:
        m_applicationProperties.getUserSettings()->setValue("language", "de");
        break;
    }

    AlertWindow::showMessageBoxAsync(
        AlertWindow::WarningIcon, TRANS("Restart required"), TRANS("Changing the language requires a restart."));
}

void EditSettingsComponent::buttonClicked(Button* clickedButton)
{
    if (clickedButton == &m_snapToGridButton)
    {
        m_applicationProperties.getUserSettings()->setValue("snapToGrid", m_snapToGridButton.getToggleState());
        m_snapToGridHeightLabel.setEnabled(m_snapToGridButton.getToggleState());
        m_snapToGridHeightEditor.setEnabled(m_snapToGridButton.getToggleState());
        m_snapToGridWidthLabel.setEnabled(m_snapToGridButton.getToggleState());
        m_snapToGridWidthEditor.setEnabled(m_snapToGridButton.getToggleState());
        m_snapToGridChanged();
    }
}

void EditSettingsComponent::textEditorTextChanged(TextEditor& editor)
{
    if (&editor == &m_snapToGridHeightEditor)
    {
        m_applicationProperties.getUserSettings()->setValue(
            "snapToGridHeight", m_snapToGridHeightEditor.getText().getIntValue());
        m_snapToGridChanged();
    }
    else if (&editor == &m_snapToGridWidthEditor)
    {
        m_applicationProperties.getUserSettings()->setValue(
            "snapToGridWidth", m_snapToGridWidthEditor.getText().getIntValue());
        m_snapToGridChanged();
    }
}
