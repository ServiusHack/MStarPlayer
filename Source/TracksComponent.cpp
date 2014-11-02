/*
  ==============================================================================

    TracksComponent.cpp
    Created: 21 Jan 2014 1:15:08am
    Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TracksComponent.h"

//==============================================================================
TracksComponent::TracksComponent(MixerComponent* mixer, int outputChannels, PositionCallback positionCallback)
: mixer(mixer), _outputChannels(outputChannels), positionCallback(positionCallback)
{
	mixer->getMixerAudioSource().addInputSource(&tracksMixer, false);
	setBounds(0,0,100,100);
}

TracksComponent::~TracksComponent()
{
	mixer->getMixerAudioSource().removeInputSource(&tracksMixer);
	tracks.clear();
}

void TracksComponent::paint (Graphics& g)
{
}

void TracksComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	int y = 0;
	int height = 100;
	for (int i = 0; i < tracks.size(); i++) {
		tracks.getUnchecked(i)->setBounds(0,y,getWidth(),height);
		y += height;
	}
	setBounds(getX(), getY(), getWidth(), y + height);
}

void TracksComponent::addMonoTrack()
{
	bool soloMute = std::any_of(tracks.begin(), tracks.end(), [](const Track* track) {
		return track->isSolo();
	});
	Track* track = new Track(tracksMixer, tracks.size() + 1, false, _outputChannels, [&]() {
		double longestDuration = 0;
		for (Track* track : tracks) {
			longestDuration = std::max(longestDuration, track->getDuration());
		}

		for (Track* track : tracks) {
			track->setLongestDuration(longestDuration);
		}
	}, soloMute, [&]() {
		bool soloMute = std::any_of(tracks.begin(), tracks.end(), [](const Track* track) {
			return track->isSolo();
		});
		for (Track* track : tracks) {
			track->setSoloMute(soloMute);
		}
	});
	addAndMakeVisible(track);
	tracks.add(track);
	resized();
}

void TracksComponent::addStereoTrack()
{
	bool soloMute = std::any_of(tracks.begin(), tracks.end(), [](const Track* track) {
		return track->isSolo();
	});
	Track* track = new Track(tracksMixer, tracks.size() + 1, true, _outputChannels, [&]() {
		double longestDuration = 0;
		Track* longestTrack = nullptr;
		for (Track* track : tracks) {
			track->setPositionCallback();
			double duration = track->getDuration();
			if (duration > longestDuration) {
				longestDuration = duration;
				longestTrack = track;
			}
		}

		longestTrack->setPositionCallback(positionCallback);

		for (Track* track : tracks) {
			track->setLongestDuration(longestDuration);
		}
	}, soloMute, [&]() {
		bool soloMute = std::any_of(tracks.begin(), tracks.end(), [](const Track* track) {
			return track->isSolo();
		});
		for (Track* track : tracks) {
			track->setSoloMute(soloMute);
		}
	});
	addAndMakeVisible(track);
	tracks.add(track);
	resized();
}

void TracksComponent::addTrackFromXml(const XmlElement& element)
{
	bool soloMute = std::any_of(tracks.begin(), tracks.end(), [](const Track* track) {
		return track->isSolo();
	});
	Track* track = new Track(tracksMixer, tracks.size() + 1, true, _outputChannels, [&]() {
		double longestDuration = 0;
		for (Track* track : tracks) {
			longestDuration = std::max(longestDuration, track->getDuration());
		}

		for (Track* track : tracks) {
			track->setLongestDuration(longestDuration);
		}
	}, soloMute, [&]() {
		bool soloMute = std::any_of(tracks.begin(), tracks.end(), [](const Track* track) {
			return track->isSolo();
		});
		for (Track* track : tracks) {
			track->setSoloMute(soloMute);
		}
	});
	tracks.add(track);
	track->restoreFromXml(element);
	addAndMakeVisible(track);
	resized();
}

void TracksComponent::play()
{
	for (int i = 0; i < tracks.size(); ++i) {
		tracks[i]->play();
	}
}

void TracksComponent::pause()
{
	for (int i = 0; i < tracks.size(); ++i) {
		tracks[i]->pause();
	}
}

void TracksComponent::stop()
{
	for (int i = 0; i < tracks.size(); ++i) {
		tracks[i]->stop();
	}
}

int TracksComponent::playerCount()
{
	return tracks.size();
}

Track& TracksComponent::player(int index)
{
	return *tracks[index];
}

void TracksComponent::setOutputChannels(int outputChannels)
{
	_outputChannels = outputChannels;
	for (int i = 0; i < tracks.size(); ++i) {
		tracks[i]->setOutputChannels(outputChannels);
	}
}