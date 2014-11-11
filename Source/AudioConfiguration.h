#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "OutputChannelNames.h"

//==============================================================================
/**
    Window wrapper for the AudioDeviceSelectorComponent.

    This allows the component to be shown in its own window.
*/
class AudioConfigurationWindow : public DialogWindow
{
public:
    AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames);
    
    /** Delete (and thus close) the window when requested by the user.
    */
    virtual void closeButtonPressed() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioConfigurationWindow)
};


class ChannelNames
	: public TableListBoxModel
{
public:
	/** A component to view and edit the channel mapping.

	@param outputChannels     Number of output channels currently configured.
	This gives the maximum value the user can select as the output channel.

	@param audioSource        The ChannelRemappingAudioSource which implements the mapping.
	It is directly modified when the user changes something.

	@param channels           Number of channels the audio source has.
	This gives the number of rows to show to the user.
	*/
	ChannelNames(OutputChannelNames& outputChannelName);

	// TableListBoxModel overrides
	virtual int getNumRows() override;
	virtual void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	virtual void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	virtual Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

	void setChannelName(int row, String text);

private:
	OutputChannelNames& m_outputChannelName;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelNames)
};

class AudioConfigurationComponent
	: public Component
	, public ButtonListener
	, public ChangeListener
{
public:
	AudioConfigurationComponent(AudioConfigurationWindow* parent, AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames);

	void resized();
	void buttonClicked(Button* buttonThatWasClicked);

	virtual void changeListenerCallback(ChangeBroadcaster *source) override;

private:
	ScopedPointer<TabbedComponent> m_tabbedComponent;
	ScopedPointer<TextButton> m_closeButton;
	ScopedPointer<ChannelNames> m_channelNames;
	ScopedPointer<TableListBox> m_tableListBox;
	

	AudioConfigurationWindow* m_parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfigurationComponent)
};