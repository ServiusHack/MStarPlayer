#include "PlayerEditDialog.h"

#include "juce_gui_extra/juce_gui_extra.h"

PlayerEditDialogWindow::PlayerEditDialogWindow(const juce::String& playerName, const juce::Colour& color,
    const juce::String& imagePath, const StringChangedCallback& stringCallback,
    const PlayerEditDialogWindow::ColourChangedCallback& colourCallback, const CloseCallback& closeCallback,
    const ImageChangedCallback& imageCallback)
    : juce::DialogWindow(TRANS("Rename player"), juce::Colours::lightgrey, true, false)
    , m_closeCallback(closeCallback)
{
    PlayerEditDialogComponent* component = new PlayerEditDialogComponent(
        playerName, color, imagePath, stringCallback, colourCallback, closeCallback, imageCallback);
    setContentOwned(component, true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(false, false);
}

void PlayerEditDialogWindow::closeButtonPressed()
{
    m_closeCallback();
}

juce::String PlayerEditDialogWindow::getPlayerName() const
{
    return static_cast<PlayerEditDialogComponent*>(getContentComponent())->m_textEditor.getText();
}

bool PlayerEditDialogWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::returnKey)
    {
        exitModalState(0);
        return true;
    }

    return false;
}

void PlayerEditDialogWindow::focusGained(juce::DialogWindow::FocusChangeType /*cause*/)
{
    static_cast<PlayerEditDialogComponent*>(getContentComponent())->m_textEditor.grabKeyboardFocus();
}

PlayerEditDialogComponent::PlayerEditDialogComponent(const juce::String& playerName, const juce::Colour& color,
    const juce::String& imagePath, const PlayerEditDialogWindow::StringChangedCallback& stringCallback,
    const PlayerEditDialogWindow::ColourChangedCallback& colourCallback,
    const PlayerEditDialogWindow::CloseCallback& closeCallback,
    const PlayerEditDialogWindow::ImageChangedCallback& imageCallback)
    : m_color(color)
    , m_colorCallback(colourCallback)
    , m_stringCallback(stringCallback)
    , m_closeCallback(closeCallback)
    , m_imageCallback(imageCallback)
    , m_imageFile(imagePath == "" ? juce::File() : juce::File(imagePath))
    , m_label("new label", TRANS("Name of the player:"))
    , m_textEditor("new text editor")
    , m_colorButton("color")
    , m_closeButton("close")
{
    addAndMakeVisible(m_label);
    m_label.setFont(juce::Font(15.00f, juce::Font::plain));
    m_label.setJustificationType(juce::Justification::centredLeft);
    m_label.setEditable(false, false, false);
    m_label.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_label.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    addAndMakeVisible(m_textEditor);
    m_textEditor.setMultiLine(false);
    m_textEditor.setReturnKeyStartsNewLine(false);
    m_textEditor.setReadOnly(false);
    m_textEditor.setScrollbarsShown(true);
    m_textEditor.setCaretVisible(true);
    m_textEditor.setPopupMenuEnabled(true);
    m_textEditor.setEscapeAndReturnKeysConsumed(false);
    m_textEditor.setText(playerName);
    m_textEditor.selectAll();
    m_textEditor.addListener(this);

    addAndMakeVisible(m_colorButton);
    m_colorButton.setButtonText(TRANS("Choose color"));
    m_colorButton.addListener(this);
    m_colorButton.setWantsKeyboardFocus(false);

    if (imageCallback)
    {
        m_imageSelectorButton.emplace("imagebutton");
        addAndMakeVisible(&m_imageSelectorButton.value());
        m_imageSelectorButton->setButtonText(TRANS("Choose image"));
        m_imageSelectorButton->addListener(this);
        m_imageSelectorButton->setWantsKeyboardFocus(false);
        m_imageSelectorButton->setConnectedEdges(juce::Button::ConnectedOnRight);

        m_imageResetButton.emplace("imageresetbutton");
        addAndMakeVisible(&m_imageResetButton.value());
        m_imageResetButton->setButtonText(TRANS("Reeset image"));
        m_imageResetButton->addListener(this);
        m_imageResetButton->setWantsKeyboardFocus(false);
        m_imageResetButton->setConnectedEdges(juce::Button::ConnectedOnLeft);
        m_imageResetButton->setEnabled(m_imageFile != juce::File());
    }

    addAndMakeVisible(m_closeButton);
    m_closeButton.setButtonText(TRANS("Close"));
    m_closeButton.addListener(this);
    m_closeButton.setWantsKeyboardFocus(false);

    setWantsKeyboardFocus(false);

    setSize(200, 200);
}

void PlayerEditDialogComponent::resized()
{
    static const int rowHeight = 24;
    static const int padding = 10;
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;

    m_label.setBounds(padding, padding, getWidth(), rowHeight);
    m_textEditor.setBounds(padding, padding + rowHeight, getWidth() - 2 * padding, rowHeight);

    m_colorButton.setBounds(padding, 2 * (padding + rowHeight), getWidth() - 2 * padding, rowHeight);

    int rowIndex = 2;
    if (m_imageSelectorButton)
    {
        int halfWidth = (getWidth() - 2 * padding) / 2;
        rowIndex++;
        m_imageSelectorButton->setBounds(padding, rowIndex * (padding + rowHeight), halfWidth, rowHeight);
        m_imageResetButton->setBounds(padding + halfWidth, rowIndex * (padding + rowHeight), halfWidth, rowHeight);
    }

    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - rowIndex * (buttonHeight - padding), buttonWidth, buttonHeight);
}

void PlayerEditDialogComponent::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &m_closeButton)
        m_closeCallback();
    else if (buttonThatWasClicked == &m_colorButton)
    {
        auto selector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showColourspace);
        selector->setName(TRANS("color chooser"));
        selector->setCurrentColour(m_color);
        selector->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
        selector->setSize(300, 400);
        selector->addChangeListener(this);

        juce::CallOutBox::launchAsynchronously(std::move(selector), m_colorButton.getScreenBounds(), nullptr);
    }
    else if (m_imageSelectorButton)
    {
        if (buttonThatWasClicked == &m_imageSelectorButton.value())
        {
            juce::FileChooser myChooser("Please select the image you want to use ...", m_imageFile, "*.jpg;*.png");

            if (!myChooser.browseForFileToOpen())
                return;

            m_imageFile = myChooser.getResult();
            m_imageCallback(m_imageFile);

            m_imageResetButton->setEnabled(m_imageFile != juce::File());
        }
        else if (buttonThatWasClicked == &m_imageResetButton.value())
        {
            m_imageFile = juce::File();
            m_imageCallback(juce::File());

            m_imageResetButton->setEnabled(m_imageFile != juce::File());
        }
    }
}

void PlayerEditDialogComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    juce::ColourSelector* selector = static_cast<juce::ColourSelector*>(source);
    m_color = selector->getCurrentColour();
    m_colorCallback(m_color);
}

void PlayerEditDialogComponent::textEditorTextChanged(juce::TextEditor&)
{
    m_stringCallback(m_textEditor.getText());
}
