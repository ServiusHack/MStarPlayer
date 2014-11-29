#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "OutputChannelNames.h"

/**
    Window wrapper for the AudioDeviceSelectorComponent.

    This allows the component to be shown in its own window.
*/
class AudioConfigurationWindow
	: public DialogWindow
	, public ButtonListener
{
public:
    AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames);
    
    /** Delete (and thus close) the window when requested by the user.
    */
	virtual void closeButtonPressed() override;

	virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioConfigurationWindow)
};


/**
	Table model for the channel names.
*/
class ChannelNames
	: public TableListBoxModel
{
public:
	ChannelNames(OutputChannelNames& outputChannelName);

	virtual int getNumRows() override;
	virtual void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	virtual void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	virtual Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

	void setChannelName(int row, String text);

private:
	OutputChannelNames& m_outputChannelName;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelNames)
};

/**
	The actual component containing the tabs.
*/
class AudioConfigurationComponent
	: public Component
	, public ChangeListener
{
public:
	AudioConfigurationComponent(AudioConfigurationWindow* parent, AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames);

	virtual void resized() override;

	virtual void changeListenerCallback(ChangeBroadcaster *source) override;

private:
	ScopedPointer<TabbedComponent> m_tabbedComponent;
	ScopedPointer<TextButton> m_closeButton;
	ScopedPointer<ChannelNames> m_channelNames;
	ScopedPointer<TableListBox> m_tableListBox;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfigurationComponent)
};