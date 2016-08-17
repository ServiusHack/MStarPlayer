#include "DarkLookAndFeel.h"

DarkLookAndFeel::DarkLookAndFeel()
{
    setColour(ResizableWindow::backgroundColourId, Colours::grey);
    setColour(PopupMenu::backgroundColourId, Colours::grey);
    setColour(AlertWindow::backgroundColourId, Colours::grey);
}