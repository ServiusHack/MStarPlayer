#pragma once

#include "juce_core/juce_core.h"

/**
        Class with some static utility functions.
*/
class Utils
{
public:
    /* Return seconds in the common mm:ss:zzz format.
     */
    static juce::String formatSeconds(double totalSeconds);

    static bool isAudioFile(const juce::String& filePath);
    static bool isImageFile(const juce::String& filePath);
};
