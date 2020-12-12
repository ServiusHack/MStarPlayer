#include "CDNamesComboBox.h"

#include "juce_audio_utils/juce_audio_utils.h"

void CDNamesComboBox::selectDrive(juce::String title)
{
    setItems();
    setText(title);
}

void CDNamesComboBox::showPopup()
{
    setItems();
    juce::ComboBox::showPopup();
}

void CDNamesComboBox::setItems()
{
    clear();

    juce::StringArray names = juce::AudioCDReader::getAvailableCDNames();

    for (int i = 0; i < names.size(); ++i)
    {
        const int itemId = i + 1; // Must not be 0.
        addItem(names[i], itemId);
        std::unique_ptr<juce::AudioCDReader> reader(juce::AudioCDReader::createReaderForCD(i));
        setItemEnabled(itemId, reader && reader->isCDStillPresent());
    }
}
