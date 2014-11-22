#include "DarkLookAndFeel.h"


DarkLookAndFeel::DarkLookAndFeel()
{
	setColour(ResizableWindow::backgroundColourId, Colours::grey);
	setColour(PopupMenu::backgroundColourId, Colours::grey);
	//setColour(PopupMenu::textColourId, Colour(0xffeeeeee));
	//setColour(PopupMenu::headerTextColourId, Colour(0xffffffff));
	//setColour(PopupMenu::highlightedTextColourId, Colour(0xffffffff));
	//setColour(PopupMenu::highlightedBackgroundColourId, Colour(0x991111aa));
	setColour(AlertWindow::backgroundColourId, Colours::grey);
	//setColour(AlertWindow::textColourId, Colour(0xffffffff));
}


AppLookAndFeel::Type DarkLookAndFeel::getType()
{
	return AppLookAndFeel::Type::Dark;
}