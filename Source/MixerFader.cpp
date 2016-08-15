#include "MixerFader.h"

#include "Player.h"

namespace {
	static int baseWidth = 70;
}

MixerFader::MixerFader(MixerControlable* mainControlable, std::vector<MixerControlable*> subControlable, bool panEnabled, ResizeCallback resizeCallback, bool soloEnabled, bool muteEnabled)
: m_label(new Label("label"))
, m_soloButton(soloEnabled ? new TextButton("solo") : nullptr)
, m_muteButton(muteEnabled ? new TextButton("mute") : nullptr)
, m_expandButton(new ChangeableArrowButton("expand", 0.0, Colour(0xff000000)))
, m_panSlider(panEnabled ? new Slider() : nullptr)
, m_levelMeter(new LevelMeter())
, m_volumeSlider(new VolumeSlider())
, m_resizeCallback(resizeCallback)
, m_mixerControlable(mainControlable)
{
	addAndMakeVisible(m_label);
	m_label->setText(mainControlable->getName(), sendNotification);
	m_label->setFont(Font(10.00f, Font::plain));
	m_label->setJustificationType(Justification::centredLeft);
	m_label->setEditable(false, false, false);
	m_label->setColour(TextEditor::textColourId, Colours::black);
	m_label->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	if (m_soloButton)
	{
		addAndMakeVisible(m_soloButton);
		m_soloButton->setButtonText(TRANS("Solo"));
		m_soloButton->addListener(this);
		m_soloButton->setWantsKeyboardFocus(false);
		m_soloButton->setConnectedEdges(TextButton::ConnectedOnRight);
		m_soloButton->setTransform(AffineTransform().scaled(0.7f));
		m_soloButton->setClickingTogglesState(true);
	}

	if (m_muteButton)
	{
		addAndMakeVisible(m_muteButton);
		m_muteButton->setButtonText(TRANS("Mute"));
		m_muteButton->addListener(this);
		m_muteButton->setWantsKeyboardFocus(false);
		m_muteButton->setConnectedEdges(TextButton::ConnectedOnLeft);
		m_muteButton->setTransform(AffineTransform().scaled(0.7f));
		m_muteButton->setClickingTogglesState(true);
	}

	if (m_panSlider)
	{
		addAndMakeVisible(m_panSlider);
		m_panSlider->setRange(0, 2, 0.1);
		m_panSlider->setValue(1.0);
		m_panSlider->setSliderStyle(Slider::LinearHorizontal);
		m_panSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
		m_panSlider->addListener(this);
		m_panSlider->setTransform(AffineTransform().scaled(0.7f));
	}

	addAndMakeVisible(m_volumeSlider);
	m_volumeSlider->setValue(m_mixerControlable->getGain(), juce::dontSendNotification);
	m_volumeSlider->addListener(this);

	addAndMakeVisible(m_levelMeter);

	addChildComponent(m_expandButton);
	m_expandButton->addListener(this);

	setSize(100, 112);

	if (m_soloButton)
	{
		setSolo(m_mixerControlable->getSolo());
	}

	if (m_muteButton)
	{
		setMute(m_mixerControlable->getMute());
	}

	setBounds(0, 0, baseWidth, 0);

	setMixSettings(subControlable);

	mainControlable->addChangeListener(this);

}

MixerFader::~MixerFader()
{
	m_mixerControlable->removeChangeListener(this);

	for (std::unique_ptr<MixerFader>& subfader : m_subfaders)
		removeChildComponent(subfader.get());
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

void MixerFader::nameChanged(const String& name)
{
	m_label->setText(name, sendNotification);
}

void MixerFader::paint(Graphics& g)
{
	g.fillAll(m_color);
}

void MixerFader::resized()
{
	static const int padding = 2;
	static const int labelHeight = 24;
	static const int buttonHeight = 24;
	static const int panHeight = 24;

	static const int buttonWidth = 50;

	m_label->setBounds(padding, padding, 2 * buttonWidth - 2 * padding, labelHeight);

	if (m_soloButton != nullptr)
	{
		m_soloButton->setBounds(padding, padding + labelHeight, buttonWidth - padding, buttonHeight);
	}

	if (m_muteButton)
	{
		m_muteButton->setBounds(padding + buttonWidth - padding, padding + labelHeight, buttonWidth - padding, buttonHeight);
	}

	if (m_panSlider)
	{
		m_panSlider->setBounds(padding, padding + labelHeight + buttonHeight, 2 * buttonWidth - 2 * padding, panHeight);
	}

	Rectangle<int> panBounds;

	if (m_panSlider)
	{
		panBounds = getLocalArea(m_panSlider, m_panSlider->getLocalBounds());
	}
	else if (m_muteButton)
	{
		panBounds = getLocalArea(m_muteButton, m_muteButton->getLocalBounds());
	}
	else if (m_soloButton)
	{
		panBounds = getLocalArea(m_soloButton, m_soloButton->getLocalBounds());
	}
	else
	{
		panBounds = getLocalArea(m_label, m_label->getLocalBounds());
	}

	m_levelMeter->setBounds(padding, panBounds.getBottomLeft().getY(), (baseWidth - 2 * padding) / 3, getHeight() - panBounds.getBottomLeft().getY() - padding);
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
		bool expand = bounds.getWidth() == baseWidth;
		bounds.setWidth(expand ? (m_subfaders.size() + 1) * baseWidth : baseWidth);
		setBounds(bounds);
		m_expandButton->setArrowDirection(expand ? 0.5f : 0.0f);
		m_resizeCallback();
	}
}

void MixerFader::sliderValueChanged(Slider* sliderThatWasMoved)
{
	if (sliderThatWasMoved == m_panSlider) {
		m_mixerControlable->setPan(static_cast<float>(sliderThatWasMoved->getValue()));
	}
	else if (sliderThatWasMoved == m_volumeSlider) {
		m_mixerControlable->setGain(static_cast<float>(sliderThatWasMoved->getValue()));
	}
}

float MixerFader::getValue() const
{
	return static_cast<float>(m_volumeSlider->getValue());
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

void MixerFader::setColor(const Colour& color)
{
	m_color = color;
	repaint();
}

void MixerFader::setLabel(const String& text)
{
	m_label->setText(text, sendNotification);
}

void MixerFader::setMixSettings(std::vector<MixerControlable*> mixSettings)
{
	for (std::unique_ptr<MixerFader>& subfader : m_subfaders) {
		removeChildComponent(subfader.get());
	}
	m_subfaders.clear();

	m_expandButton->setVisible(mixSettings.size() > 0);
	for (size_t i = 0; i < mixSettings.size(); ++i) {
		m_subfaders.emplace_back(new MixerFader(mixSettings[i], std::vector<MixerControlable*>(), false, [](){}));
		addAndMakeVisible(m_subfaders.back().get());
	}

	// resize
	Rectangle<int> bounds = getBounds();
	if (bounds.getWidth() != baseWidth) {
		bounds.setWidth((m_subfaders.size() + 1) * baseWidth);
		setBounds(bounds);
		m_resizeCallback();
	}
}

void MixerFader::updateLevel()
{
	m_levelMeter->setVolume(m_mixerControlable->getVolume());
	for (auto&& fader : m_subfaders)
		fader->updateLevel();
}