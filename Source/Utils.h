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

	template<class T>
	static bool absoluteCompare(T a, T b) {
		return std::abs(a) < std::abs(b);
	}
};

