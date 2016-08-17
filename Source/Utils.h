#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
	Class with some static utility functions.
*/
class Utils
{
public:
    /* Return seconds in the common mm:ss:zzz format.
	 */
    static String formatSeconds(double totalSeconds);
};
