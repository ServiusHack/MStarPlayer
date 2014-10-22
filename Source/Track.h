/*
  ==============================================================================

	Track.h
	Created: 21 Jan 2014 1:00:48am
	Author:  User

  ==============================================================================
*/

#ifndef TRACK_H_INCLUDED
#define TRACK_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Track    : public Component
{
public:
	Track(MixerAudioSource &tracksMixer, bool stereo, int outputChannels);
	~Track();

	void paint (Graphics&);
	void resized();

	void mouseDown (const MouseEvent & event);

	void play();
	void pause();
	void stop();

	std::vector<int> getMapping();
	int getNumChannels();

	void setOutputChannels(int outputChannels);
	void setOutputChannelMapping(int source, int target);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)

	void loadFile();

	ScopedPointer<Label> idLabel;
	ScopedPointer<Label> descriptionLabel;
	ScopedPointer<TextButton> editButton;
	ScopedPointer<TextButton> openButton;
	ScopedPointer<TextButton> soloButton;
	ScopedPointer<TextButton> muteButton;
	ScopedPointer<AudioThumbnail> audioThumbnail;
	ScopedPointer<ChannelRemappingAudioSource> remappingAudioSource;
	AudioThumbnailCache audioThumbnailCache;
	AudioFormatManager formatManager;

	File audioFile;
	bool stereo;

	//AudioFormatManager formatManager;
	MixerAudioSource &tracksMixer;
	TimeSliceThread thread;
	AudioTransportSource transportSource;
	AudioFormatReaderSource* currentAudioFileSource;
};


#endif  // TRACK_H_INCLUDED
