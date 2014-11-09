#include "RenameDialog.h"

RenameDialogWindow::RenameDialogWindow(String playerName, Colour color, String imagePath, StringChangedCallback stringCallback, RenameDialogWindow::ColourChangedCallback colourCallback, CloseCallback closeCallback, ImageChangedCallback imageCallback)
	: DialogWindow("Rename player", Colours::lightgrey, true, false)
	, m_closeCallback(closeCallback)
{
	RenameDialogComponent* component = new RenameDialogComponent(playerName, color, imagePath, this, stringCallback, colourCallback, closeCallback, imageCallback);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(false, false);
}

void RenameDialogWindow::closeButtonPressed()
{
	m_closeCallback();
}

String RenameDialogWindow::getPlayerName()
{
	return static_cast<RenameDialogComponent*>(getContentComponent())->m_textEditor->getText();
}

bool RenameDialogWindow::keyPressed(const KeyPress &key)
{
	if (key == KeyPress::returnKey) {
		exitModalState(0);
		return true;
	}

	return false;
}

void RenameDialogWindow::focusGained(FocusChangeType /*cause*/)
{
	static_cast<RenameDialogComponent*>(getContentComponent())->m_textEditor->grabKeyboardFocus();
}

RenameDialogComponent::RenameDialogComponent(String playerName, Colour color, String imagePath, RenameDialogWindow* parent, RenameDialogWindow::StringChangedCallback stringCallback, RenameDialogWindow::ColourChangedCallback colourCallback, RenameDialogWindow::CloseCallback closeCallback, RenameDialogWindow::ImageChangedCallback imageCallback)
	: m_parent(parent)
	, m_color(color)
	, m_colorCallback(colourCallback)
	, m_stringCallback(stringCallback)
	, m_closeCallback(closeCallback)
	, m_imageCallback(imageCallback)
	, m_imageFile(imagePath == "" ? File::nonexistent : File(imagePath))
{
	addAndMakeVisible(m_label = new Label("new label",
                                          TRANS("Name of the player:")));
	m_label->setFont(Font(15.00f, Font::plain));
	m_label->setJustificationType(Justification::centredLeft);
	m_label->setEditable(false, false, false);
	m_label->setColour(TextEditor::textColourId, Colours::black);
	m_label->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	addAndMakeVisible(m_textEditor = new TextEditor("new text editor"));
	m_textEditor->setMultiLine(false);
	m_textEditor->setReturnKeyStartsNewLine(false);
	m_textEditor->setReadOnly(false);
	m_textEditor->setScrollbarsShown(true);
	m_textEditor->setCaretVisible(true);
	m_textEditor->setPopupMenuEnabled(true);
	m_textEditor->setEscapeAndReturnKeysConsumed(false);
	m_textEditor->setText(playerName);
	m_textEditor->selectAll();
	m_textEditor->addListener(this);

	addAndMakeVisible(m_colorButton = new TextButton("color"));
	m_colorButton->setButtonText(TRANS("Choose color"));
	m_colorButton->addListener(this);
	m_colorButton->setWantsKeyboardFocus(false);

	if (imageCallback) {
		m_imageSelectorButton.set(new TextButton("imagebutton"), true);
		addAndMakeVisible(m_imageSelectorButton.get());
		m_imageSelectorButton->setButtonText(TRANS("Choose image"));
		m_imageSelectorButton->addListener(this);
		m_imageSelectorButton->setWantsKeyboardFocus(false);
		m_imageSelectorButton->setConnectedEdges(Button::ConnectedOnRight);

		m_imageResetButton.set(new TextButton("imageresetbutton"), true);
		addAndMakeVisible(m_imageResetButton.get());
		m_imageResetButton->setButtonText(TRANS("Reeset image"));
		m_imageResetButton->addListener(this);
		m_imageResetButton->setWantsKeyboardFocus(false);
		m_imageResetButton->setConnectedEdges(Button::ConnectedOnLeft);
		m_imageResetButton->setEnabled(m_imageFile != File::nonexistent);
	}

	addAndMakeVisible(m_closeButton = new TextButton("close"));
	m_closeButton->setButtonText(TRANS("Close"));
	m_closeButton->addListener(this);
	m_closeButton->setWantsKeyboardFocus(false);

	setWantsKeyboardFocus(false);

    setSize(200, 200);
}

RenameDialogComponent::~RenameDialogComponent()
{
	m_label = nullptr;
	m_textEditor = nullptr;
	m_closeButton = nullptr;
}

void RenameDialogComponent::resized()
{
	static const int rowHeight = 24;
	static const int padding = 10;
	const static int buttonWidth = 80;
	const static int buttonHeight = 24;

	m_label->setBounds(padding, padding, getWidth(), rowHeight);
	m_textEditor->setBounds(padding, padding + rowHeight, getWidth() - 2 * padding, rowHeight);

	m_colorButton->setBounds(padding, 2 * (padding + rowHeight), getWidth() - 2 * padding, rowHeight);

	int rowIndex = 2;
	if (m_imageSelectorButton) {
		int halfWidth = (getWidth() - 2 * padding) / 2;
		rowIndex++;
		m_imageSelectorButton->setBounds(padding, rowIndex * (padding + rowHeight), halfWidth, rowHeight);
		m_imageResetButton->setBounds(padding + halfWidth, rowIndex * (padding + rowHeight), halfWidth, rowHeight);
	}

	m_closeButton->setBounds(
		(getWidth() - buttonWidth) / 2,
		getHeight() - rowIndex * (buttonHeight - padding),
		buttonWidth,
		buttonHeight
		);
}

void RenameDialogComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_closeButton)
		m_closeCallback();
	else if (buttonThatWasClicked == m_imageSelectorButton.get())
	{
		FileChooser myChooser("Please select the image you want to use ...",
			m_imageFile,
			"*.jpg;*.png");

		if (!myChooser.browseForFileToOpen())
			return;

		m_imageFile = myChooser.getResult();
		m_imageCallback(m_imageFile);

		m_imageResetButton->setEnabled(m_imageFile != File::nonexistent);
	}
	else if (buttonThatWasClicked == m_imageResetButton)
	{
		m_imageFile = File::nonexistent;
		m_imageCallback(File::nonexistent);

		m_imageResetButton->setEnabled(m_imageFile != File::nonexistent);
	}
	else if (buttonThatWasClicked == m_colorButton)
	{
		ColourSelector* selector =new ColourSelector(ColourSelector::showColourspace);
		selector->setName("color chooser");
		selector->setCurrentColour(m_color);
		selector->setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
		selector->setSize(300, 400);
		selector->addChangeListener(this);

		CallOutBox::launchAsynchronously(selector, m_colorButton->getScreenBounds(), nullptr);
	}
}

void RenameDialogComponent::changeListenerCallback(ChangeBroadcaster *source)
{
	ColourSelector* selector = static_cast<ColourSelector*>(source);
	m_color = selector->getCurrentColour();
	m_colorCallback(m_color);
}

void RenameDialogComponent::textEditorTextChanged(TextEditor &)
{
	m_stringCallback(m_textEditor->getText());
}