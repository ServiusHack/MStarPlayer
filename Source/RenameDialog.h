#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

class RenameDialogWindow : public DialogWindow
{
public:

	typedef std::function<void(Colour)> ColourChangedCallback;
	typedef std::function<void(String)> StringChangedCallback;
	typedef std::function<void()> CloseCallback;

	RenameDialogWindow(String playerName, Colour color, StringChangedCallback stringCallback, ColourChangedCallback colourCallback, CloseCallback closeCallback);

	void closeButtonPressed();

	String getPlayerName();

	bool keyPressed(const KeyPress &key);

	void focusGained(FocusChangeType cause);

private:
	CloseCallback m_closeCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RenameDialogWindow)
};

class RenameDialogComponent
	: public Component
	, public ButtonListener
	, public ChangeListener
	, public juce::TextEditor::Listener
{
	friend class RenameDialogWindow;

public:
	RenameDialogComponent(String playerName, Colour color, RenameDialogWindow* parent, RenameDialogWindow::StringChangedCallback stringCallback, RenameDialogWindow::ColourChangedCallback colourCallback, RenameDialogWindow::CloseCallback closeCallback);
	~RenameDialogComponent();

    void resized();
    void buttonClicked(Button* buttonThatWasClicked);

	virtual void changeListenerCallback(ChangeBroadcaster *source) override;
	virtual void textEditorTextChanged(TextEditor &) override;

private:
	ScopedPointer<Label> m_label;
	ScopedPointer<TextEditor> m_textEditor;
	ScopedPointer<TextButton> m_closeButton;

	ScopedPointer<TextButton> m_colorButton;

	RenameDialogWindow* m_parent;
	Colour m_color;

	RenameDialogWindow::ColourChangedCallback m_colorCallback;
	RenameDialogWindow::StringChangedCallback m_stringCallback;

	RenameDialogWindow::CloseCallback m_closeCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenameDialogComponent)
};
