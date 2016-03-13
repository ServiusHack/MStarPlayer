#include "PlaylistEntry.h"

XmlElement* PlaylistEntry::saveToXml(const File& projectDirectory) const
{
	XmlElement* entryXml = new XmlElement("Entry");
	
	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(name);
	entryXml->addChildElement(nameXml);

	entryXml->setAttribute("playNext", playNext);

	XmlElement* trackConfigsXml = new XmlElement("TrackConfigs");

	for (size_t i = 0; i < trackConfigs.size(); ++i) {
		XmlElement* trackConfigXml = new XmlElement("TrackConfig");
		XmlElement* fileXml = new XmlElement("File");
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
PlaylistEntry PlaylistEntry::createFromXml(const XmlElement& element, const File& projectDirectory)
{
	PlaylistEntry entry;

	entry.name = element.getChildByName("Name")->getAllSubText().trim();
	entry.playNext = element.getBoolAttribute("playNext");

	XmlElement* trackConfigsXml = element.getChildByName("TrackConfigs");

	for (int i = 0; i < trackConfigsXml->getNumChildElements(); ++i) {
		TrackConfig config;
		config.file = File(projectDirectory.getChildFile(trackConfigsXml->getChildElement(i)->getChildByName("File")->getAllSubText().trim()));
		entry.trackConfigs.push_back(config);
	}

	return entry;
}
