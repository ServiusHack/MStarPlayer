#pragma once

#include <set>

#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerComponent.h"
#include "MixerControlable.h"

#include "PlaylistPlayerWindow.h"
#include "SubchannelPlayer.h"
#include "PlaylistModel.h"
#include "JinglePlayerWindow.h"
#include "InterPlayerCommunication.h"


class Player : public Component, public SubchannelPlayer {

public:
	Player(MixerComponent* mixer, OutputChannelNames *outputChannelNames, InterPlayerCommunication::PlayerType type, ApplicationProperties& applicationProperties, float gain = 1.0f, bool solo = false, bool mute = false);
	~Player();

	void setType(InterPlayerCommunication::PlayerType type);

	virtual void resized() override;

	virtual void setGain(float gain) override;

	virtual float getGain() const override;

	virtual void setPan(float pan) override;

	virtual float getPan() const override;

	virtual void setSoloMute(bool soloMute) override;

	virtual bool getSoloMute() const override;

	virtual void setSolo(bool solo) override;

	virtual bool getSolo() const override;

	virtual void setMute(bool mute) override;

	virtual bool getMute() const override;

	virtual float getVolume() const override;

    /** Set the number of output channels.

        If the user reconfigures his audio settings the number of output channels
        might change. This method is called to propagate this change to this player.
    */
    void setOutputChannels(int outputChannels);
	
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
	XmlElement* saveToXml() const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
	void restoreFromXml(const XmlElement& element);
	
	virtual void SetChannelCountChangedCallback(Track::ChannelCountChangedCallback callback) override;

	virtual std::vector<MixerControlable*> getSubMixerControlables() const override;


private:

	void updateGain();

	void setColor(Colour color);

	void showEditDialog();
	void configureChannels();

	MixerComponent* m_mixer;
	OutputChannelNames* m_outputChannelNames;

	PlaylistModel playlistModel;
	TracksContainer m_tracksContainer;

	PlaylistPlayerWindow m_playlistPlayer;
	JinglePlayerWindow m_jinglePlayer;

	float m_gain;
	bool m_soloMute;
	bool m_solo;
	bool m_mute;
	InterPlayerCommunication::PlayerType m_type;

	Colour m_color;

	Track::ChannelCountChangedCallback m_channelCountChanged;
	OptionalScopedPointer<PlayerEditDialogWindow> m_PlayerEditDialog;
	OptionalScopedPointer<ChannelMappingWindow> m_channelMappingWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Player)
};
