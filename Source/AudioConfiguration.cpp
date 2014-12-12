#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConfiguration.h"

class ChannelNameTextEditor
	: public TextEditor
{
public:
	ChannelNameTextEditor()
		: m_row(0)
	{
		setBoundsInset(BorderSize<int>(2));
	}

	void setRow(const int newRow)
	{
		m_row = newRow;
	}

	int getRow() const
	{
		return m_row;
	}

private:
	int m_row;
};

AudioConfigurationWindow::AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames)
	: DialogWindow("Configure Audio", Colours::lightgrey, true, true)
{
	setContentOwned(new AudioConfigurationComponent(this, audioDeviceManager, outputChannelNames), true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(true, true);
}

void AudioConfigurationWindow::closeButtonPressed()
{
	setVisible(false);
}

void AudioConfigurationWindow::buttonClicked(Button*)
{
	closeButtonPressed();
}

ChannelNames::ChannelNames(OutputChannelNames& outputChannelName)
	: m_outputChannelName(outputChannelName)
{
}

int ChannelNames::getNumRows()
{
	return m_outputChannelName.getNumberOfChannels();
}

void ChannelNames::paintRowBackground(Graphics& /*g*/, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{
}

void ChannelNames::paintCell(Graphics& g,
	int rowNumber,
	int columnId,
	int width, int height,
	bool /*rowIsSelected*/)
{
	g.setColour(Colours::black);

	if (columnId == 1)
		g.drawText(m_outputChannelName.getDeviceOutputChannelName(rowNumber), 2, 0, width - 4, height, Justification::centredLeft, true);

	g.setColour(Colours::black.withAlpha(0.2f));
	g.fillRect(width - 1, 0, 1, height);
}

Component* ChannelNames::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
	if (columnId == 2) {
		ChannelNameTextEditor* editor = static_cast<ChannelNameTextEditor*>(existingComponentToUpdate);

		if (editor == nullptr)
		{
			editor = new ChannelNameTextEditor();
			editor->addListener(this);
		}

		editor->setRow(rowNumber);
		editor->setText(m_outputChannelName.getInternalOutputChannelName(rowNumber), dontSendNotification);

		return editor;
	}
	else {
		jassert(existingComponentToUpdate == 0);
		return 0;
	}
}

void ChannelNames::textEditorTextChanged(TextEditor& textEditor)
{
	ChannelNameTextEditor& channelNameTextEditor = static_cast<ChannelNameTextEditor&>(textEditor);
	m_outputChannelName.setInternalOutputChannelName(channelNameTextEditor.getRow(), channelNameTextEditor.getText());
}

AudioConfigurationComponent::AudioConfigurationComponent(AudioConfigurationWindow* parent, AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelName)
	: m_channelNames(new ChannelNames(outputChannelName))
{
	outputChannelName.addChangeListener(this);

	addAndMakeVisible(m_tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop));

	AudioDeviceSelectorComponent* selector = new AudioDeviceSelectorComponent(audioDeviceManager, 0, 0, 1, 64, false, false, false, false);
	m_tabbedComponent->addTab("Audio Device", Colour(0xffffffff), selector, true);
	
	m_tableListBox = new TableListBox();
	m_tabbedComponent->addTab("Channel Names", Colour(0xffffffff), m_tableListBox, true);
	m_tableListBox->setColour(ListBox::outlineColourId, Colours::grey);
	m_tableListBox->setOutlineThickness(1);
	m_tableListBox->setModel(m_channelNames);

	// set the table header columns
	m_tableListBox->getHeader().addColumn("Device Channel", 1, 200, 50, 400, TableHeaderComponent::defaultFlags);
	m_tableListBox->getHeader().addColumn("Channel Name", 2, 200, 50, 400, TableHeaderComponent::defaultFlags);

	addAndMakeVisible(m_closeButton = new TextButton("close"));
	m_closeButton->setButtonText(TRANS("Close"));
	m_closeButton->addListener(parent);
	m_closeButton->setWantsKeyboardFocus(false);

	setSize(500, 400);

	changeListenerCallback(&outputChannelName);
}

void AudioConfigurationComponent::resized()
{
	const static int buttonWidth = 80;
	const static int buttonHeight = 24;
	const static int padding = 10;

	m_tabbedComponent->setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - buttonHeight - 3 * padding);
	m_closeButton->setBounds(
		(getWidth() - buttonWidth) / 2,
		getHeight() - buttonHeight - padding,
		buttonWidth,
		buttonHeight
	);
}

void AudioConfigurationComponent::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
	m_tableListBox->updateContent();
}