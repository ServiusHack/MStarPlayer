#include "PlaylistEntry.h"

juce::XmlElement* PlaylistEntry::saveToXml(const juce::File& projectDirectory) const
{
    juce::XmlElement* entryXml = new juce::XmlElement("Entry");

    juce::XmlElement* nameXml = new juce::XmlElement("Name");
    nameXml->addTextElement(name);
    entryXml->addChildElement(nameXml);

    entryXml->setAttribute("playNext", playNext);

    juce::XmlElement* trackConfigsXml = new juce::XmlElement("TrackConfigs");

    for (size_t i = 0; i < trackConfigs.size(); ++i)
    {
        juce::XmlElement* trackConfigXml = new juce::XmlElement("TrackConfig");
        juce::XmlElement* fileXml = new juce::XmlElement("File");
        if (trackConfigs[i].file.isAChildOf(projectDirectory))
            fileXml->addTextElement(trackConfigs[i].file.getRelativePathFrom(projectDirectory));
        else
            fileXml->addTextElement(trackConfigs[i].file.getFullPathName());
        trackConfigXml->addChildElement(fileXml);
        trackConfigsXml->addChildElement(trackConfigXml);
    }

    entryXml->addChildElement(trackConfigsXml);

    return entryXml;
}
PlaylistEntry PlaylistEntry::createFromXml(const juce::XmlElement& element, const juce::File& projectDirectory)
{
    PlaylistEntry entry;

    entry.name = element.getChildByName("Name")->getAllSubText().trim();
    entry.playNext = element.getBoolAttribute("playNext");

    juce::XmlElement* trackConfigsXml = element.getChildByName("TrackConfigs");

    for (int i = 0; i < trackConfigsXml->getNumChildElements(); ++i)
    {
        TrackConfig config;
        const juce::String filename
            = trackConfigsXml->getChildElement(i)->getChildByName("File")->getAllSubText().trim();
        if (!filename.isEmpty())
            config.file = juce::File(projectDirectory.getChildFile(filename));
        entry.trackConfigs.push_back(config);
    }

    return entry;
}
