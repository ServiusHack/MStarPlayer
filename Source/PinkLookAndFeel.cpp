#include "PinkLookAndFeel.h"

PinkLookAndFeel::PinkLookAndFeel()
{
    setColour(ResizableWindow::backgroundColourId, Colours::hotpink);
    setColour(PopupMenu::backgroundColourId, Colours::hotpink);
    setColour(AlertWindow::backgroundColourId, Colours::hotpink);
}
