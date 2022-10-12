#include "MTCSender.h"

#include <math.h>

#include "juce_gui_basics/juce_gui_basics.h"

MTCSender::MTCSender()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}

MTCSender::~MTCSender() {}

void MTCSender::setDevices(juce::Array<juce::MidiDeviceInfo> deviceInfos)
{
    outputs.erase(std::remove_if(outputs.begin(),
                      outputs.end(),
                      [&deviceInfos](const std::unique_ptr<juce::MidiOutput>& output)
                      {
                          // Remove devices that are no longer requested.
                          // At the same time remove already open devices from the list of devices to open.
                          bool deviceWasRequested = deviceInfos.removeAllInstancesOf(output->getDeviceInfo()) > 0;
                          return !deviceWasRequested;
                      }),
        outputs.end());

    juce::StringArray failedDeviceNames;
    for (const juce::MidiDeviceInfo& info : deviceInfos)
    {
        auto device = juce::MidiOutput::openDevice(info.identifier);
        if (device)
            outputs.push_back(std::move(device));
        else
            failedDeviceNames.add(info.name);
    }

    if (!failedDeviceNames.isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            TRANS("Failed opening MIDI output devices"),
            TRANS("The following MIDI devices could not be opened:\n") + failedDeviceNames.joinIntoString("\n"));
    }
}

juce::Array<juce::MidiDeviceInfo> MTCSender::getDevices()
{
    juce::Array<juce::MidiDeviceInfo> infos;
    for (auto& output : outputs)
    {
        infos.add(output->getDeviceInfo());
    }
    return infos;
}

void MTCSender::start()
{
    m_frame = 0;
    m_second = 0;
    m_minute = 0;
    m_hour = 0;

    auto message = juce::MidiMessage::fullFrame(
        m_hour, m_minute, m_second, m_frame, juce::MidiMessage::SmpteTimecodeType::fps25);
    for (auto& output : outputs)
    {
        output->sendMessageNow(message);
    }

    startTimer(1000 / 25 / 4);
}

void MTCSender::pause()
{
    if (isTimerRunning())
        stopTimer();
    else
        startTimer(1000 / 25 / 4);
}

void MTCSender::stop()
{
    stopTimer();
}

void MTCSender::setPosition(double position)
{
    std::unique_lock<std::mutex> lock_guard(m_mutex);
    double unused;

    m_frame = static_cast<int>(modf(position, &unused) * 25);
    m_second = static_cast<int>(position) % 60;
    m_minute = (static_cast<int>(position) / 60) % 60;
    m_hour = (static_cast<int>(position) / 60 / 60) % 60;
    m_piece = Piece::FrameLSB;

    auto message = juce::MidiMessage::fullFrame(
        m_hour, m_minute, m_second, m_frame, juce::MidiMessage::SmpteTimecodeType::fps25);
    for (auto& output : outputs)
    {
        output->sendMessageNow(message);
    }
}

void MTCSender::hiResTimerCallback()
{
    if (outputs.empty())
        return;

    std::unique_lock<std::mutex> lock_guard(m_mutex);

    const int value = getValue(m_piece);
    const auto message = juce::MidiMessage::quarterFrame(static_cast<int>(m_piece), value);
    for (auto& output : outputs)
    {
        output->sendMessageNow(message);
    }

    m_piece = static_cast<Piece>((static_cast<int>(m_piece) + 1) % 8);

    if (++m_quarter >= 4)
    {
        m_quarter = 0;
        if (++m_frame >= 25)
        {
            m_frame = 0;
            if (++m_second >= 60)
            {
                m_second = 0;
                if (++m_minute >= 60)
                {
                    m_minute = 0;
                    if (++m_hour >= 24)
                    {
                        m_hour = 0;
                    }
                }
            }
        }
    }
}

int MTCSender::getValue(Piece piece)
{
    switch (piece)
    {
    case Piece::FrameLSB:
        return m_frame & 0b1111;
    case Piece::FrameMSB:
        return (m_frame >> 4) & 0b0001;
    case Piece::SecondLSB:
        return m_second & 0b1111;
    case Piece::SecondMSB:
        return (m_second >> 4) & 0b0011;
    case Piece::MinuteLSB:
        return m_minute & 0b1111;
    case Piece::MinuteMSB:
        return (m_minute >> 4) & 0b0011;
    case Piece::HourLSB:
        return m_hour & 0b1111;
    case Piece::RateAndHourMSB:
        return ((m_hour >> 4) & 0b0001) | (0b01 << 1);
    }

    std::terminate();
}
