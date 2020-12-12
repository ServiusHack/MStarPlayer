#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

class TestToneGeneratorOutputChannelsModel
    : public juce::ListBoxModel
    , public juce::TextButton::Listener
{
public:
    using ChangeCallback = std::function<void(int, bool)>;
    int getNumRows() override;

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForRow(
        int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate);

    void resizeTo(int size);

    void setChannelName(int pos, juce::String name);
    ChangeCallback m_changeCallback;

    void selectAll();
    void deselectAll();

private:
    struct Data
    {
        juce::String name;
        bool enabled{false};
    };
    juce::Array<Data> names;

    // Inherited via Listener
    virtual void buttonClicked(juce::Button*) override;
};
