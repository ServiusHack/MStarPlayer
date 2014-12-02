#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

class PlayerEditDialogWindow : public DialogWindow
{
public:

	typedef std::function<void(Colour)> ColourChangedCallback;
	typedef std::function<void(String)> StringChangedCallback;
	typedef std::function<void(juce::File)> ImageChangedCallback;
	typedef std::function<void()> CloseCallback;

	PlayerEditDialogWindow(String playerName, Colour color, String imagePath, StringChangedCallback stringCallback, ColourChangedCallback colourCallback, CloseCallback closeCallback, ImageChangedCallback imageCallback = ImageChangedCallback());

	void closeButtonPressed();

	String getPlayerName() const;

	virtual bool keyPressed(const KeyPress &key) override;

	virtual void focusGained(FocusChangeType cause) override;

private:
	CloseCallback m_closeCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerEditDialogWindow)
};

class PlayerEditDialogComponent
	: public Component
	, public ButtonListener
	, public ChangeListener
	, public juce::TextEditor::Listener
{
	friend class PlayerEditDialogWindow;

public:
	PlayerEditDialogComponent(String playerName, Colour color, String imagePath, PlayerEditDialogWindow::StringChangedCallback stringCallback, PlayerEditDialogWindow::ColourChangedCallback colourCallback, PlayerEditDialogWindow::CloseCallback closeCallback, PlayerEditDialogWindow::ImageChangedCallback imageCallback);
	~PlayerEditDialogComponent();

	virtual void resized() override;
	virtual void buttonClicked(Button* buttonThatWasClicked) override;

	virtual void changeListenerCallback(ChangeBroadcaster *source) override;
	virtual void textEditorTextChanged(TextEditor &) override;

private:
	ScopedPointer<Label> m_label;
	ScopedPointer<TextEditor> m_textEditor;
	ScopedPointer<TextButton> m_closeButton;

	ScopedPointer<TextButton> m_colorButton;
	OptionalScopedPointer<TextButton> m_imageSelectorButton;
	OptionalScopedPointer<TextButton> m_imageResetButton;

	Colour m_color;
	File m_imageFile;

	PlayerEditDialogWindow::ColourChangedCallback m_colorCallback;
	PlayerEditDialogWindow::StringChangedCallback m_stringCallback;
	PlayerEditDialogWindow::ImageChangedCallback m_imageCallback;

	PlayerEditDialogWindow::CloseCallback m_closeCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerEditDialogComponent)
};
