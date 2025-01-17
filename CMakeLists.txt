cmake_minimum_required(VERSION 3.12)

if (NOT VERSION)
	message(WARNING "No VERSION given, defaulting to 0.0.0")
	set(VERSION "0.0.0")
endif()

cmake_policy(SET CMP0091 NEW)

project(MStarPlayer VERSION ${VERSION})

add_subdirectory(JUCE)

juce_add_gui_app(MStarPlayer)

set_property(TARGET MStarPlayer PROPERTY CXX_STANDARD 17)

set_property(TARGET MStarPlayer PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_sources(MStarPlayer PRIVATE
    Source/AudioConfiguration.cpp
    Source/AudioConfiguration.h
    Source/AudioDeviceComponent.cpp
    Source/AudioDeviceComponent.h
    Source/CDNamesComboBox.cpp
    Source/CDNamesComboBox.h
    Source/CDPlayer.cpp
    Source/CDPlayer.h
    Source/CDTracksModel.cpp
    Source/CDTracksModel.h
    Source/CDTracksTable.cpp
    Source/CDTracksTable.h
    Source/ChangeableArrowButton.cpp
    Source/ChangeableArrowButton.h
    Source/ChannelMappingDialog.cpp
    Source/ChannelMappingDialog.h
    Source/ChannelMixerControlable.cpp
    Source/ChannelMixerControlable.h
    Source/ChannelMixerFader.cpp
    Source/ChannelMixerFader.h
    Source/ChannelRemappingAudioSourceWithVolume.cpp
    Source/ChannelRemappingAudioSourceWithVolume.h
    Source/ChannelVolumeAudioSource.cpp
    Source/ChannelVolumeAudioSource.h
    Source/CrashDumper.cpp
    Source/CrashDumper.h
    Source/DarkLookAndFeel.cpp
    Source/DarkLookAndFeel.h
    Source/DefaultLookAndFeel.cpp
    Source/DefaultLookAndFeel.h
    Source/EditSettingsDialog.cpp
    Source/EditSettingsDialog.h
    Source/InterPlayerCommunication.h
    Source/JinglePlayerWindow.cpp
    Source/JinglePlayerWindow.h
    Source/LevelMeter.cpp
    Source/LevelMeter.h
    Source/MTCSender.cpp
    Source/MTCSender.h
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/MainComponent.h
    Source/MidiConfiguration.cpp
    Source/MidiConfiguration.h
    Source/MixerComponent.cpp
    Source/MixerComponent.h
    Source/MixerControlable.h
    Source/MixerFader.cpp
    Source/MixerFader.h
    Source/MyMultiDocumentPanel.cpp
    Source/MyMultiDocumentPanel.h
    Source/OutputChannelNames.cpp
    Source/OutputChannelNames.h
    Source/PanSlider.cpp
    Source/PanSlider.h
    Source/PinkLookAndFeel.cpp
    Source/PinkLookAndFeel.h
    Source/Player.cpp
    Source/Player.h
    Source/PlayerComponent.h
    Source/PlayerEditDialog.cpp
    Source/PlayerEditDialog.h
    Source/PlayerMidiDialog.cpp
    Source/PlayerMidiDialog.h
    Source/PlayerMixerFader.cpp
    Source/PlayerMixerFader.h
    Source/PlaylistEntry.cpp
    Source/PlaylistEntry.h
    Source/PlaylistEntryDialog.cpp
    Source/PlaylistEntryDialog.h
    Source/PlaylistModel.cpp
    Source/PlaylistModel.h
    Source/PlaylistPlayerWindow.cpp
    Source/PlaylistPlayerWindow.h
    Source/PlaylistTable.cpp
    Source/PlaylistTable.h
    Source/PluginInterface.h
    Source/PluginInterfaceV1.h
    Source/PluginInterfaceV2.h
    Source/PluginInterfaceV3.h
    Source/PluginLoader.cpp
    Source/PluginLoader.h
    Source/SoloBusComponent.cpp
    Source/SoloBusComponent.h
    Source/SoloBusMixer.cpp
    Source/SoloBusMixer.h
    Source/SoloBusSettings.cpp
    Source/SoloBusSettings.h
    Source/SubchannelPlayer.h
    Source/TestToneGeneratorAudioSource.cpp
    Source/TestToneGeneratorAudioSource.h
    Source/TestToneGeneratorComponent.cpp
    Source/TestToneGeneratorComponent.h
    Source/TestToneGeneratorOutputChannelsModel.cpp
    Source/TestToneGeneratorOutputChannelsModel.h
    Source/Track.cpp
    Source/Track.h
    Source/TrackEditDialog.cpp
    Source/TrackEditDialog.h
    Source/TrackUi.cpp
    Source/TrackUi.h
    Source/TracksComponent.cpp
    Source/TracksComponent.h
    Source/TracksContainer.cpp
    Source/TracksContainer.h
    Source/Utils.cpp
    Source/Utils.h
    Source/VolumeAnalyzer.cpp
    Source/VolumeAnalyzer.h
    Source/VolumeSlider.cpp
    Source/VolumeSlider.h
    Source/Waveform.cpp
    Source/Waveform.h
)

target_compile_definitions(MStarPlayer PRIVATE
    _CRT_SECURE_NO_WARNINGS
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0 
    JUCE_USE_CDREADER=1
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:MStarPlayer,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:MStarPlayer,JUCE_VERSION>"
)

if (ASIO_PATH)
target_compile_definitions(MStarPlayer PRIVATE
    JUCE_ASIO=1
)
target_include_directories(MStarPlayer PRIVATE
    ${ASIO_PATH}
)
endif()

juce_add_binary_data(BinaryData SOURCES 
    Icons/oxygen/media-eject.png
    Icons/oxygen/arrow-right-double.png
    Icons/oxygen/audio-volume-medium.png
    Icons/oxygen/configure.png
    Icons/oxygen/audio-headphones.png
    Icons/oxygen/audio-volume-muted.png
    Icons/oxygen/media-playback-pause.png
    Icons/oxygen/media-playback-start.png
    Icons/oxygen/media-playback-stop.png
    Icons/oxygen/media-skip-backward.png
    Icons/oxygen/media-skip-forward.png
    Icons/stop.svg
    Icons/play.svg
    Translations/German.txt
)

set_property(TARGET BinaryData PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(MStarPlayer PRIVATE
    BinaryData
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_cryptography
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_video
)
