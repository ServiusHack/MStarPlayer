#pragma once

#include <mutex>

#include "juce_audio_devices/juce_audio_devices.h"

class MTCSender : juce::HighResolutionTimer
{
public:
    MTCSender();
    ~MTCSender();

    void setDevices(juce::Array<juce::MidiDeviceInfo> deviceInfos);
    juce::Array<juce::MidiDeviceInfo> getDevices();
    void start();
    void pause();
    void stop();
    void setPosition(double position);

private:
    std::vector<std::unique_ptr<juce::MidiOutput>> outputs;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MTCSender)

    // Used only in separate thread!
private:
    enum class Piece
    {
        FrameLSB = 0,
        FrameMSB,
        SecondLSB,
        SecondMSB,
        MinuteLSB,
        MinuteMSB,
        HourLSB,
        RateAndHourMSB
    };
    void hiResTimerCallback() override;
    int getValue(Piece piece);

    std::mutex m_mutex;
    Piece m_piece{Piece::FrameLSB};
    int m_quarter{0};
    int m_frame{0};
    int m_second{0};
    int m_minute{0};
    int m_hour{0};
};
