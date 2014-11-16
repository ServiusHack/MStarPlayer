/*
  ==============================================================================

    MixerFader.cpp
    Created: 8 Nov 2014 1:40:21pm
    Author:  User

  ==============================================================================
*/

#include "MixerFader.h"

#include "Player.h"

namespace {
	static int baseWidth = 70;
}

MixerFader::MixerFader(MixerControlable* mainControlable, std::vector<MixerControlable*> subControlable, bool panEnabled, ResizeCallback resizeCallback)
: m_label(new Label("label", "C1"))
, m_soloButton(new TextButton("solo"))
, m_muteButton(new TextButton("mute"))
, m_expandButton(new ArrowButton("expand", 0.0, Colour(0xff000000)))
, m_panSlider(panEnabled ? new Slider() : nullptr)
, m_volumeSlider(new Slider())
, m_resizeCallback(resizeCallback)
, m_mixerControlable(mainControlable)
{
	addAndMakeVisible(m_label);
	m_label->setFont(Font(10.00f, Font::plain));
	m_label->setJustificationType(Justification::centredLeft);
	m_label->setEditable(false, false, false);
	m_label->setColour(TextEditor::textColourId, Colours::black);
	m_label->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	addAndMakeVisible(m_soloButton);
	m_soloButton->setButtonText(TRANS("Solo"));
	m_soloButton->addListener(this);
	m_soloButton->setWantsKeyboardFocus(false);
	m_soloButton->setConnectedEdges(TextButton::ConnectedOnRight);
	m_soloButton->setTransform(AffineTransform().scaled(0.7));
	m_soloButton->setClickingTogglesState(true);

	addAndMakeVisible(m_muteButton);
	m_muteButton->setButtonText(TRANS("Mute"));
	m_muteButton->addListener(this);
	m_muteButton->setWantsKeyboardFocus(false);
	m_muteButton->setConnectedEdges(TextButton::ConnectedOnLeft);
	m_muteButton->setTransform(AffineTransform().scaled(0.7));
	m_muteButton->setClickingTogglesState(true);

	if (m_panSlider)
	{
		addAndMakeVisible(m_panSlider);
		m_panSlider->setRange(0, 2, 0.1);
		m_panSlider->setValue(1.0);
		m_panSlider->setSliderStyle(Slider::LinearHorizontal);
		m_panSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
		m_panSlider->addListener(this);
		m_panSlider->setTransform(AffineTransform().scaled(0.7));
	}

	addAndMakeVisible(m_volumeSlider);
	m_volumeSlider->setRange(0, 2, 0.1);
	m_volumeSlider->setValue(m_mixerControlable->getGain());
	m_volumeSlider->setSliderStyle(Slider::LinearVertical);
	m_volumeSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	m_volumeSlider->addListener(this);

	addChildComponent(m_expandButton);
	m_expandButton->addListener(this);

	setSize(100, 112);

	setSolo(m_mixerControlable->getSolo());
	setMute(m_mixerControlable->getMute());

	setBounds(0, 0, baseWidth, 0);

	setMixSettings(subControlable);

	mainControlable->addChangeListener(this);
}

MixerFader::~MixerFader()
{
	m_mixerControlable->removeChangeListener(this);
}

void MixerFader::gainChanged(float gain)
{
	m_volumeSlider->setValue(gain);
}

void MixerFader::panChanged(float pan)
{
	m_panSlider->setValue(pan);
}

void MixerFader::soloChanged(bool solo)
{
	m_soloButton->setToggleState(solo, sendNotification);
}

void MixerFader::muteChanged(bool mute)
{
	m_muteButton->setToggleState(mute, sendNotification);
}

void MixerFader::paint(Graphics& g)
{
	g.fillAll(m_color);
	/*g.setColour(Colour(0xff000000));
	Rectangle<int> b = getLocalBounds();
	//b.reduce(1,1);
	g.drawRect(b);

	Rectangle<int> soloBounds = m_soloButton->getLocalBounds();
	Rectangle<int> localArea = getLocalArea(m_soloButton, soloBounds);
	g.setColour(Colour(0xffff0000));
	g.fillRect(localArea);*/
	/*g.setColour(Colour(0xffff0000));
	Rectangle<int> panBounds = getLocalArea(m_muteButton, m_muteButton->getLocalBounds());
	g.fillRect(panBounds);*/
}

void MixerFader::resized()
{
	static const int padding = 2;
	static const int labelHeight = 24;
	static const int buttonHeight = 24;
	static const int panHeight = 24;

	static const int buttonWidth = 50;

	m_soloButton->setBounds(2, 2, 8, 8);

	m_label->setBounds(padding, padding, 2 * buttonWidth - 2 * padding, labelHeight);
	m_soloButton->setBounds(padding, padding + labelHeight, buttonWidth - padding, buttonHeight);
	m_muteButton->setBounds(padding + buttonWidth - padding, padding + labelHeight, buttonWidth - padding, buttonHeight);

	Rectangle<int> panBounds;

	if (m_panSlider == nullptr)
	{
		panBounds = getLocalArea(m_muteButton, m_muteButton->getLocalBounds());
	}
	else
	{
		m_panSlider->setBounds(padding, padding + labelHeight + buttonHeight, 2 * buttonWidth - 2 * padding, panHeight);
		panBounds = getLocalArea(m_panSlider, m_panSlider->getLocalBounds());
	}

	m_volumeSlider->setBounds(padding + (baseWidth - 2 * padding) / 3, panBounds.getBottomLeft().getY(), (baseWidth - 2 * padding) / 3, getHeight() - panBounds.getBottomLeft().getY() - padding);
	m_expandButton->setBounds(padding + 2 * (baseWidth - 2 * padding) / 3, panBounds.getBottomLeft().getY(), (baseWidth - 2 * padding) / 3, getHeight() - panBounds.getBottomLeft().getY() - padding);

	for (size_t i = 0; i < m_subfaders.size(); ++i)
		m_subfaders[i]->setBounds((i + 1) * baseWidth, 0, baseWidth, getHeight());
}

void MixerFader::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_soloButton) {
		m_mixerControlable->setSolo(m_soloButton->getToggleState());
	}
	else if (buttonThatWasClicked == m_muteButton) {
		m_mixerControlable->setMute(m_muteButton->getToggleState());
	}
	else if (buttonThatWasClicked == m_expandButton) {
		Rectangle<int> bounds = getBounds();
		bounds.setWidth(bounds.getWidth() == baseWidth ? (m_subfaders.size() + 1) * baseWidth : baseWidth);
		setBounds(bounds);
		m_resizeCallback();
	}
}

void MixerFader::sliderValueChanged(Slider* sliderThatWasMoved)
{
	if (sliderThatWasMoved == m_panSlider) {
		m_mixerControlable->setPan(sliderThatWasMoved->getValue());
	}
	else if (sliderThatWasMoved == m_volumeSlider) {
		m_mixerControlable->setGain(sliderThatWasMoved->getValue());
	}
}

float MixerFader::getValue()
{
	return m_volumeSlider->getValue();
}

void MixerFader::setValue(float value)
{
	m_volumeSlider->setValue(value);
}

void MixerFader::setMute(bool mute)
{
	m_muteButton->setToggleState(mute, sendNotification);
}

void MixerFader::setSolo(bool solo)
{
	m_soloButton->setToggleState(solo, sendNotification);
}

void MixerFader::setColor(Colour color)
{
	m_color = color;
	repaint();
}

void MixerFader::setLabel(String text)
{
	m_label->setText(text, sendNotification);
}

void MixerFader::setMixSettings(std::vector<MixerControlable*> mixSettings)
{
	for (MixerFader* subfader : m_subfaders) {
		removeChildComponent(subfader);
		delete subfader;
	}
	m_subfaders.clear();

	m_expandButton->setVisible(mixSettings.size() > 0);
	for (int i = 0; i < mixSettings.size(); ++i) {
		MixerFader* fader = new MixerFader(mixSettings[i], std::vector<MixerControlable*>(), false, [](){});
		addAndMakeVisible(fader);
		m_subfaders.push_back(fader);
	}

	// resize
	Rectangle<int> bounds = getBounds();
	if (bounds.getWidth() != baseWidth) {
		bounds.setWidth((m_subfaders.size() + 1) * baseWidth);
		setBounds(bounds);
		m_resizeCallback();
	}
}