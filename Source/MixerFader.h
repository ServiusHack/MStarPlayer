#pragma once

#include <functional>
#include <memory>

#include "ChangeableArrowButton.h"
#include "LevelMeter.h"
#include "MixerControlable.h"
#include "VolumeSlider.h"

/**
        A set of controls for an player channel or output channel.

        This includes:
        - volume fader
        - level meter
        - solo button
        - mute button
        - pan slider (not fully implemented yet)
*/
class MixerFader
    : public juce::Component
    , public juce::Button::Listener
    , public juce::Slider::Listener
    , public MixerControlableChangeListener
{
public:
    using SoloChangedCallback = std::function<void(bool)>;
    using ResizeCallback = std::function<void()>;

    MixerFader(MixerControlable* mainControlable, std::vector<MixerControlable*> subControlable, bool panEnabled,
        ResizeCallback resizeCallback, bool soloEnabled = true, bool muteEnabled = true);
    ~MixerFader();

    float getValue() const;
    void setValue(float value);

    void setMute(bool mute);
    void setSolo(bool solo);

    void setColor(const juce::Colour& color);

    void setLabel(const juce::String& text);

    void setMixSettings(std::vector<MixerControlable*> mixSettings);

    void updateLevel();

    void pairTo(MixerFader* fader);

    // Component
public:
    virtual void paint(juce::Graphics&) override;
    virtual void resized() override;

    // Button::Listener
public:
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

    // Slider::Listener
public:
    virtual void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;

    // MixerControlableChangeListener
public:
    virtual void gainChanged(float gain) override;

    virtual void panChanged(float pan) override;

    virtual void soloChanged(bool solo) override;

    virtual void muteChanged(bool mute) override;

    virtual void nameChanged(const juce::String& name) override;

protected:
    std::unique_ptr<VolumeSlider> m_volumeSlider;

private:
    juce::Label m_label;
    std::unique_ptr<juce::TextButton> m_soloButton;
    std::unique_ptr<juce::TextButton> m_muteButton;
    ChangeableArrowButton m_expandButton;
    std::unique_ptr<juce::Slider> m_panSlider;
    LevelMeter m_levelMeter;

    ResizeCallback m_resizeCallback;

    std::vector<std::unique_ptr<MixerFader>> m_subfaders;

    MixerControlable* m_mixerControlable;

    juce::Colour m_color;

    MixerFader* m_pairedFader{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerFader)
};
