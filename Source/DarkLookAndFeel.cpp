#include "DarkLookAndFeel.h"

DarkLookAndFeel::DarkLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::grey);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colours::grey);
    setColour(juce::AlertWindow::backgroundColourId, juce::Colours::grey);
}
