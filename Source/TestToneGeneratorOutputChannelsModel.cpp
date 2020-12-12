#include "TestToneGeneratorOutputChannelsModel.h"

int TestToneGeneratorOutputChannelsModel::getNumRows()
{
    return names.size();
}

void TestToneGeneratorOutputChannelsModel::paintListBoxItem(
    int /*rowNumber*/, juce::Graphics& /*g*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{
}

juce::Component* TestToneGeneratorOutputChannelsModel::refreshComponentForRow(
    int rowNumber, bool /*isRowSelected*/, juce::Component* existingComponentToUpdate)
{
    if (rowNumber >= names.size())
    {
        if (existingComponentToUpdate)
            delete existingComponentToUpdate;
        return nullptr;
    }

    juce::ToggleButton* button = static_cast<juce::ToggleButton*>(existingComponentToUpdate);

    if (!button)
    {
        button = new juce::ToggleButton();
        button->addListener(this);
    }

    button->setToggleState(names[rowNumber].enabled, juce::dontSendNotification);
    button->getProperties().set("row", rowNumber);

    button->setButtonText(names[rowNumber].name);
    return button;
}

void TestToneGeneratorOutputChannelsModel::resizeTo(int size)
{
    names.resize(size);
}

void TestToneGeneratorOutputChannelsModel::setChannelName(int pos, juce::String name)
{
    names.getReference(pos).name = name;
}

void TestToneGeneratorOutputChannelsModel::buttonClicked(juce::Button* button)
{
    int pos = button->getProperties()["row"];
    names.getReference(pos).enabled = button->getToggleState();
    m_changeCallback(pos, names[pos].enabled);
}

void TestToneGeneratorOutputChannelsModel::selectAll()
{
    for (int i = 0; i < names.size(); ++i)
    {
        if (names[i].enabled)
            continue;

        m_changeCallback(i, true);
        names.getReference(i).enabled = true;
    }
}

void TestToneGeneratorOutputChannelsModel::deselectAll()
{
    for (int i = 0; i < names.size(); ++i)
    {
        if (!names[i].enabled)
            continue;

        m_changeCallback(i, false);
        names.getReference(i).enabled = false;
    }
}
