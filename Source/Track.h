/*
  ==============================================================================

	Track.h
	Created: 21 Jan 2014 1:00:48am
	Author:  User

  ==============================================================================
*/

#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

typedef std::function<void()> DurationChangedCallback;

//==============================================================================
/*
*/
class Track	: public Component
			, public ChangeListener
            , public Button::Listener
{
public:
	Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback);
	~Track();

	/** Play or stop the audio playback. */
	void buttonClicked(Button * /*button*/);

	void paint (Graphics&);
	void resized();

	void mouseDown (const MouseEvent & event);

	void play();
	void pause();
	void stop();

	double getDuration();

	std::vector<int> getMapping();
	int getNumChannels();

	void setOutputChannels(int outputChannels);
	void setOutputChannelMapping(int source, int target);

	void changeListenerCallback(ChangeBroadcaster *source);

	void setLongestDuration(double duration);
	
	// Track should be muted because other track(s) are in solo mode.
	void setSoloMute(bool mute);

	bool isSolo() const;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)

	void updateIdText();
	void loadFile();
	void setMuteState();

	ScopedPointer<Label> idLabel;
	ScopedPointer<Label> descriptionLabel;
	ScopedPointer<TextButton> editButton;
	ScopedPointer<DrawableButton> openButton;
	ScopedPointer<Drawable> openImage;
	ScopedPointer<DrawableButton> soloButton;
	ScopedPointer<Drawable> soloImage;
	ScopedPointer<DrawableButton> muteButton;
	ScopedPointer<Drawable> muteImage;
	ScopedPointer<AudioThumbnail> audioThumbnail;
	ScopedPointer<ChannelRemappingAudioSource> remappingAudioSource;
	AudioThumbnailCache audioThumbnailCache;
	AudioFormatManager formatManager;

	File audioFile;
	bool stereo;

	int trackIndex;

	//AudioFormatManager formatManager;
	MixerAudioSource &tracksMixer;
	TimeSliceThread thread;
	AudioTransportSource transportSource;
	AudioFormatReaderSource* currentAudioFileSource;

	bool m_soloMute;
	double m_duration;
	double m_longestDuration;

	DurationChangedCallback durationChangedCallback;
	DurationChangedCallback soloChangedCallback;
};


#endif  // TRACK_H_INCLUDED
