#include "juce_audio_basics/juce_audio_basics.h"

#include "TestToneGeneratorComponent.h"

TestToneGeneratorComponent::TestToneGeneratorComponent(
    MixerComponent* mixerComponent, OutputChannelNames* outputChannelNames)
    : m_mixerComponent(mixerComponent)
    , m_outputChannelNames(outputChannelNames)
{
    const juce::FontOptions fontOptions(15.00f, juce::Font::plain);
    m_outputChannelNames->addListener(this);
    m_outputChannelNames->addChangeListener(this);

    comboBox.reset(new juce::ComboBox("new combo box"));
    addAndMakeVisible(comboBox.get());
    comboBox->setEditableText(false);
    comboBox->setJustificationType(juce::Justification::centredLeft);
    comboBox->setTextWhenNothingSelected(juce::String());
    comboBox->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    comboBox->addItem(TRANS("1kHz"), 1);
    comboBox->addItem(TRANS("White"), 2);
    comboBox->addListener(this);

    comboBox->setBounds(64, 16, 112, 24);

    toggleButton.reset(new juce::TextButton("new button"));
    addAndMakeVisible(toggleButton.get());
    toggleButton->setButtonText(TRANS("play"));
    toggleButton->addListener(this);

    toggleButton->setBounds(200, 16, 86, 24);

    component.reset(new juce::ListBox());
    addAndMakeVisible(component.get());
    component->setName("new component");

    label.reset(new juce::Label("new label", TRANS("Tone:")));
    addAndMakeVisible(label.get());
    label->setFont(juce::Font(fontOptions).withTypefaceStyle("Regular"));
    label->setJustificationType(juce::Justification::centredLeft);
    label->setEditable(false, false, false);
    label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    label->setBounds(8, 16, 64, 24);

    slider.reset(new VolumeSlider());
    addAndMakeVisible(slider.get());
    slider->setName("new slider");

    slider->setBounds(8, 56, 40, 296);

    textButton.reset(new juce::TextButton("new button"));
    addAndMakeVisible(textButton.get());
    textButton->setButtonText(TRANS("select all"));
    textButton->setConnectedEdges(juce::Button::ConnectedOnRight);
    textButton->addListener(this);

    textButton2.reset(new juce::TextButton("new button"));
    addAndMakeVisible(textButton2.get());
    textButton2->setButtonText(TRANS("select none"));
    textButton2->setConnectedEdges(juce::Button::ConnectedOnLeft);
    textButton2->addListener(this);

    dbLabel.reset(new juce::Label("new label", TRANS("0 dB")));
    addAndMakeVisible(dbLabel.get());
    dbLabel->setFont(juce::Font(fontOptions).withTypefaceStyle("Regular"));
    dbLabel->setJustificationType(juce::Justification::centredLeft);
    dbLabel->setEditable(false, false, false);
    dbLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    dbLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    dbLabel->setBounds(8, 360, 280, 24);

    toggleButton->setEnabled(false);
    toggleButton->setWantsKeyboardFocus(false);
    toggleButton->setClickingTogglesState(true);

    setSize(296, 400);

    component->setMultipleSelectionEnabled(true);
    component->setModel(&m_listBoxModel);
    m_listBoxModel.resizeTo(m_outputChannelNames->getNumberOfChannels());
    for (int i = 0; i < m_outputChannelNames->getNumberOfChannels(); i++)
    {
        m_listBoxModel.setChannelName(i, m_outputChannelNames->getInternalOutputChannelName(i));
    }
    m_audioSource.setNumChannels(m_outputChannelNames->getNumberOfChannels());
    component->updateContent();

    m_listBoxModel.m_changeCallback = [this](int row, bool enable)
    {
        component->selectRow(row, true, false);
        m_audioSource.setEnable(row, enable);
    };

    slider->addListener(this);
    slider->setValue(m_audioSource.getVolume());
}

TestToneGeneratorComponent::~TestToneGeneratorComponent()
{
    m_outputChannelNames->removeListener(this);
    m_outputChannelNames->removeChangeListener(this);
    m_mixerComponent->getMixerAudioSource().removeInputSource(&m_audioSource);
    slider->removeListener(this);

    comboBox = nullptr;
    toggleButton = nullptr;
    component = nullptr;
    label = nullptr;
    slider = nullptr;
    textButton = nullptr;
    textButton2 = nullptr;
    dbLabel = nullptr;
}

void TestToneGeneratorComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void TestToneGeneratorComponent::resized()
{
    component->setBounds(56, 56, getWidth() - 64, getHeight() - 144);
    textButton->setBounds(56, 320, 112, 24);
    textButton2->setBounds(168, 320, 118, 24);
}

void TestToneGeneratorComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBox.get())
    {
        if (comboBoxThatHasChanged->isEnabled())
            toggleButton->setEnabled(true);

        if (comboBoxThatHasChanged->getSelectedId() == 1)
            m_audioSource.setMode(TestToneGeneratorAudioSource::Mode::Sine1kHz);
        else if (comboBoxThatHasChanged->getSelectedId() == 2)
            m_audioSource.setMode(TestToneGeneratorAudioSource::Mode::White);
    }
}

void TestToneGeneratorComponent::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == toggleButton.get())
    {
        if (toggleButton->getToggleState())
        {
            m_mixerComponent->getMixerAudioSource().addInputSource(&m_audioSource, false);
        }
        else
        {
            m_mixerComponent->getMixerAudioSource().removeInputSource(&m_audioSource);
        }
    }
    else if (buttonThatWasClicked == textButton.get())
    {
        m_listBoxModel.selectAll();
        component->updateContent();
    }
    else if (buttonThatWasClicked == textButton2.get())
    {
        m_listBoxModel.deselectAll();
        component->updateContent();
    }
}

void TestToneGeneratorComponent::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    int numberOfChannels = m_outputChannelNames->getNumberOfChannels();
    int oldSize = m_listBoxModel.getNumRows();
    m_listBoxModel.resizeTo(numberOfChannels);
    for (int i = oldSize; i < numberOfChannels; ++i)
    {
        m_listBoxModel.setChannelName(i, m_outputChannelNames->getInternalOutputChannelName(i));
    }
    m_audioSource.setNumChannels(m_outputChannelNames->getNumberOfChannels());
    component->updateContent();
}

void TestToneGeneratorComponent::outputChannelNamesReset() {}

void TestToneGeneratorComponent::outputChannelNameChanged(int activeChannelIndex, const juce::String& text)
{
    m_listBoxModel.setChannelName(activeChannelIndex, text);
    component->updateContent();
}

void TestToneGeneratorComponent::outputChannelPairingModeChanged(int /*activeChannelIndex*/, PairingMode /*mode*/) {}

void TestToneGeneratorComponent::sliderValueChanged(juce::Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == slider.get())
    {
        m_audioSource.setVolume(sliderThatWasMoved->getValue());
        dbLabel->setText(juce::Decibels::toString(juce::Decibels::gainToDecibels(sliderThatWasMoved->getValue())),
            juce::sendNotification);
    }
}
