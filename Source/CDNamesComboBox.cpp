#include "CDNamesComboBox.h"

void CDNamesComboBox::showPopup()
{
    clear();

    StringArray names = AudioCDReader::getAvailableCDNames();

    for (int i = 0; i < names.size(); ++i)
    {
        const int itemId = i+1; // Must not be 0.
        addItem(names[i], itemId);
        ScopedPointer<AudioCDReader> reader(AudioCDReader::createReaderForCD(i));
        setItemEnabled(itemId, reader && reader->isCDStillPresent());
    }

    ComboBox::showPopup();
}