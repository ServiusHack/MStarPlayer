#include "Utils.h"

String Utils::formatSeconds(double totalSeconds)
{
	int minutes = ((int)totalSeconds / 60) % (60);
	int seconds = ((int)totalSeconds) % 60;
	int milliseconds = ((int)(totalSeconds * 1000)) % 1000;

	return String::formatted("%02i:%02i:%03i", minutes, seconds, milliseconds);
}
