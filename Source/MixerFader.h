#pragma once

#include <functional>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"

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
    : public Component
    , public ButtonListener
    , public juce::Slider::Listener
    , public MixerControlableChangeListener
{
public:
    typedef std::function<void(bool)> SoloChangedCallback;
    typedef std::function<void()> ResizeCallback;

    MixerFader(MixerControlable* mainControlable, std::vector<MixerControlable*> subControlable, bool panEnabled,
        ResizeCallback resizeCallback, bool soloEnabled = true, bool muteEnabled = true);
    ~MixerFader();

    float getValue() const;
    void setValue(float value);

    void setMute(bool mute);
    void setSolo(bool solo);

    void setColor(const Colour& color);

    void setLabel(const String& text);

    void setMixSettings(std::vector<MixerControlable*> mixSettings);

    void updateLevel();

    void pairTo(MixerFader* fader);

    // Component
public:
    virtual void paint(Graphics&) override;
    virtual void resized() override;

    // ButtonListener
public:
    virtual void buttonClicked(Button* buttonThatWasClicked) override;

    // Slider::Listener
public:
    virtual void sliderValueChanged(Slider* sliderThatWasMoved) override;

    // MixerControlableChangeListener
public:
    virtual void gainChanged(float gain) override;

    virtual void panChanged(float pan) override;

    virtual void soloChanged(bool solo) override;

    virtual void muteChanged(bool mute) override;

    virtual void nameChanged(const String& name) override;

protected:
    std::unique_ptr<VolumeSlider> m_volumeSlider;

private:
    std::unique_ptr<Label> m_label;
    std::unique_ptr<TextButton> m_soloButton;
    std::unique_ptr<TextButton> m_muteButton;
    std::unique_ptr<ChangeableArrowButton> m_expandButton;
    std::unique_ptr<Slider> m_panSlider;
    std::unique_ptr<LevelMeter> m_levelMeter;

    ResizeCallback m_resizeCallback;

    std::vector<std::unique_ptr<MixerFader>> m_subfaders;

    MixerControlable* m_mixerControlable;

    Colour m_color;

    MixerFader* m_pairedFader{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerFader)
};
