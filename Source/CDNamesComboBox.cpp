#include "CDNamesComboBox.h"

void CDNamesComboBox::selectDrive(String title)
{
    setItems();
    setText(title);
}

void CDNamesComboBox::showPopup()
{
    setItems();
    ComboBox::showPopup();
}

void CDNamesComboBox::setItems()
{
    clear();

    StringArray names = AudioCDReader::getAvailableCDNames();

    for (int i = 0; i < names.size(); ++i)
    {
        const int itemId = i + 1; // Must not be 0.
        addItem(names[i], itemId);
        ScopedPointer<AudioCDReader> reader(AudioCDReader::createReaderForCD(i));
        setItemEnabled(itemId, reader && reader->isCDStillPresent());
    }
}