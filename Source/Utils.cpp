#include "Utils.h"

#include "juce_audio_formats/juce_audio_formats.h"

juce::String Utils::formatSeconds(double doubleTotalSeconds)
{
    const int totalSeconds = static_cast<int>(doubleTotalSeconds);
    const int minutes = (totalSeconds / 60) % (60);
    const int seconds = (totalSeconds) % 60;
    const int milliseconds = (static_cast<int>(doubleTotalSeconds * 1000)) % 1000;

    return juce::String::formatted("%02i:%02i:%03i", minutes, seconds, milliseconds);
}

bool Utils::isAudioFile(const juce::String& filePath)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
    {
        for (auto&& extension : formatManager.getKnownFormat(i)->getFileExtensions())
        {
            if (filePath.endsWithIgnoreCase(extension))
            {
                return true;
            }
        }
    }

    return false;
}

bool Utils::isImageFile(const juce::String& filePath)
{
    return filePath.endsWithIgnoreCase(".jpg") || filePath.endsWithIgnoreCase(".png");
}
