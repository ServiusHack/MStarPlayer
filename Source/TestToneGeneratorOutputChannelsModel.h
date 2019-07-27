#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class TestToneGeneratorOutputChannelsModel
    : public ListBoxModel
    , public TextButton::Listener
{
public:
    using ChangeCallback = std::function<void(int, bool)>;
    int getNumRows() override;

    void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate);

    void resizeTo(int size);

    void setChannelName(int pos, String name);
    ChangeCallback m_changeCallback;

    void selectAll();
    void deselectAll();

private:
    struct Data
    {
        String name;
        bool enabled{false};
    };
    Array<Data> names;

    // Inherited via Listener
    virtual void buttonClicked(Button*) override;
};
