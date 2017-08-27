#include "Utils.h"

String Utils::formatSeconds(double doubleTotalSeconds)
{
    const int totalSeconds = static_cast<int>(doubleTotalSeconds);
    const int minutes = (totalSeconds / 60) % (60);
    const int seconds = (totalSeconds) % 60;
    const int milliseconds = (static_cast<int>(doubleTotalSeconds * 1000)) % 1000;

    return String::formatted("%02i:%02i:%03i", minutes, seconds, milliseconds);
}
