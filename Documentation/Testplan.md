# Test plan

This document describes simple steps for manual testing which will cover quite a lot of the functionality and code base.

## Configure Audio

Open the Options -> Configure Audio dialog and try the following things:

1. Unselect some active output channels and verify they are removed from:
   * the channel names tab
   * the solo bus tab
   * the mixer at the bottom of the main window
2. Switch the output device and verify the channels are updated in the above listed locations
3. Select an audio device with at least 4 output channels and configure it as follows:
   * two channels as stereo pair (mode: left of below / right of above)
   * one channel as left channel of solo bus
   * one channel as right channel of solo bus

## Player Playback

1. Load one file into a Jingle Player
2. Load two files as a playlist into the Playlist Player
3. Enable 'play next' for all files in the Playlist Player
4. Play both players and check that:
   * Both play audio
   * The audio is mixed correctly
   * The playlist player loops at least once through all files
5. For the playlist player try the next, previous, pause and stop buttons

## CD Player Playback

1. Insert a CD into your CD drive
2. Add a CD player
3. Check it lists all tracks and their durations
4. Play the CD and verify it plays correctly
5. Try the next, previous, pause and stop buttons

## Player Control

For a CD player and another player:

1. Try the mute button of one player
2. Try the solo button of one player
3. Pan one player to the left and right
4. Change the volume of one player
5. Change the volume of a channel
6. Change the channel configuration of a player while it is playing

## Generate Test Tones

1. Open the Test Tone Generator
2. Choose one channel, a tone, and play it
3. Change the volume
4. Change the test tone
5. Change the selected channels

## Project Saving and Loading

1. Save the project after following the above steps
2. Restart M\*Player
3. Open the project and verify:
   * audio is configured as before
   * players are configured as before
   * playback still works

## Change Appearances

1. Switch the Stype (Standard, Dark or Pink)
2. Change the name and color of a player
3. Load an image into a Jingle Player
4. Switch between the Tabs view and the Windows view
5. Enable 'snap players to grid' in the settings with a grid size greater than 1x1
6. Verify players snap to the grid when being moved
