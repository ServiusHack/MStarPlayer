#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConfiguration.h"

class ChannelNameCustomComponent
	: public Component
	, public TextEditor::Listener
{
public:
	ChannelNameCustomComponent(ChannelNames& owner)
		: m_owner(owner)
	{
		// just put a combo box inside this component
		addAndMakeVisible(&m_textEditor);

		// when the combo is changed, we'll get a callback.
		m_textEditor.addListener(this);
	}

	virtual void resized() override
	{
		m_textEditor.setBoundsInset(BorderSize<int>(2));
	}

	void setRowAndColumn(const int newRow, String text)
	{
		m_row = newRow;
		m_textEditor.setText(text, dontSendNotification);
	}

	virtual void textEditorTextChanged(TextEditor &) override
	{
		m_owner.setChannelName(m_row, m_textEditor.getText());
	}

private:
	ChannelNames& m_owner;
	TextEditor m_textEditor;
	int m_row, m_columnId;
};

AudioConfigurationWindow::AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames)
	: DialogWindow ("Configure Audio", Colours::lightgrey, true, true)
{
	AudioConfigurationComponent* component = new AudioConfigurationComponent(this, audioDeviceManager, outputChannelNames);
	setContentOwned(component, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
	setResizable(true, true);
}

void AudioConfigurationWindow::closeButtonPressed()
{
	setVisible(false);
}

void AudioConfigurationWindow::buttonClicked(Button* /*buttonThatWasClicked*/)
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

	if (columnId == 1) {
		g.drawText(m_outputChannelName.getDeviceOutputChannelName(rowNumber), 2, 0, width - 4, height, Justification::centredLeft, true);
	}

	g.setColour(Colours::black.withAlpha(0.2f));
	g.fillRect(width - 1, 0, 1, height);
}

Component* ChannelNames::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
	if (columnId == 2) {
		// If it's the output channel column, we'll return our custom component..

		ChannelNameCustomComponent* editor = static_cast<ChannelNameCustomComponent*>(existingComponentToUpdate);

		// If an existing component is being passed-in for updating, we'll re-use it, but
		// if not, we'll have to create one.
		if (editor == nullptr)
			editor = new ChannelNameCustomComponent(*this);

		editor->setRowAndColumn(rowNumber, m_outputChannelName.getInternalOutputChannelName(rowNumber));

		return editor;
	}
	else {
		// for any other column, just return 0, as we'll be painting these columns directly.

		jassert(existingComponentToUpdate == 0);
		return 0;
	}
}

void ChannelNames::setChannelName(int row, String text)
{
	m_outputChannelName.setInternalOutputChannelName(row, text);
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