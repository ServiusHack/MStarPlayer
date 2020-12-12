#include "PinkLookAndFeel.h"

#include "juce_gui_basics/juce_gui_basics.h"

PinkLookAndFeel::PinkLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::hotpink);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colours::hotpink);
    setColour(juce::AlertWindow::backgroundColourId, juce::Colours::hotpink);
}
