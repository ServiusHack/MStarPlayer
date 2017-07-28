#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

enum class PairingMode
{
    Mono = 0,
    Left,
    Right
};

/**
	Interface to receive changes to the output channel names.
*/
class OutputChannelNamesListener
{
public:
    virtual void outputChannelNamesReset() = 0;
    virtual void outputChannelNameChanged(int activeChannelIndex, const String& text) = 0;
    virtual void outputChannelPairingModeChanged(int activeChannelIndex, PairingMode mode) = 0;
};

/**
	Identification of an audio device.

	We can't use a pointer to the audio device because the AudioDeviceManager might delete, reuse or do what
	it wants to with it. Therefore we take the type name and device name which is most likely unique to each
	device in the system.
*/
struct DeviceIdentification
{
    String deviceTypeName;
    String deviceName;

    inline bool operator!=(const DeviceIdentification& other);
};

/**
	Maintains the user defined output channel names.
*/
class OutputChannelNames
    : public ChangeListener
    , public ChangeBroadcaster
{
public:
    OutputChannelNames(AudioDeviceManager& deviceManager);

    int getNumberOfChannels();

private:
    DeviceIdentification ExtractDeviceIdentification(AudioIODevice* audioDevice);
    AudioIODevice* m_audioDevice;
    DeviceIdentification m_deviceIdentification;

// channel pairing
public:
    bool SetChannelPairing(int activeChannelIndex, PairingMode mode);
    PairingMode GetChannelPairing(int activeChannelIndex);

private:
    Array<PairingMode> m_pairingModes;

// output device names and their internal names
public:
    String getDeviceOutputChannelName(int activeChannelIndex);
    StringArray getAllDeviceOutputChannelNames();
    String getInternalOutputChannelName(int activeChannelIndex);
    void setInternalOutputChannelName(int activeChannelIndex, const String& text);

private:
    StringArray m_deviceOutputChannelNames;
    StringArray m_internalOutputChannelNames;

// XML serialization
public:
    void saveToXml(XmlElement* element) const;
    void restoreFromXml(const XmlElement& element);

// OutputChannelNamesListener management
public:
    /**
		Add a listener which will be notified when a channel name changes or all names are reset.

		The ownership will not be transfered, it is the caller's responsibility to remove and destroy the listener later.
	*/
    void addListener(OutputChannelNamesListener* listener);

    void removeListener(OutputChannelNamesListener* listener);

private:
    std::vector<OutputChannelNamesListener*> m_listeners;

// ChangeListener
public:
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;
};