#include "PlaylistModel.h"

#include "Utils.h"


XmlElement* PlaylistEntry::saveToXml() const
{
	XmlElement* entryXml = new XmlElement("Entry");
	
	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(name);
	entryXml->addChildElement(nameXml);

	XmlElement* trackConfigsXml = new XmlElement("TrackConfigs");

	for (int i = 0; i < trackConfigs.size(); ++i) {
		XmlElement* trackConfigXml = new XmlElement("TrackConfig");
		XmlElement* fileXml = new XmlElement("File");
		fileXml->addTextElement(trackConfigs.getUnchecked(i).file.getFullPathName());
		trackConfigXml->addChildElement(fileXml);
		trackConfigsXml->addChildElement(trackConfigXml);
	}

	entryXml->addChildElement(trackConfigsXml);

	return entryXml;
}
PlaylistEntry PlaylistEntry::createFromXml(const XmlElement& element)
{
	PlaylistEntry entry;

	entry.name = element.getChildByName("Name")->getAllSubText().trim();

	XmlElement* trackConfigsXml = element.getChildByName("TrackConfigs");

	for (int i = 0; i < trackConfigsXml->getNumChildElements(); ++i) {
		TrackConfig config;
		config.file = File(trackConfigsXml->getChildElement(i)->getChildByName("File")->getAllSubText().trim());
		entry.trackConfigs.add(config);
	}

	return entry;
}

PlaylistModel::PlaylistModel()
{
}

int PlaylistModel::getNumRows()
{
	return m_playlist.size();
}

void PlaylistModel::paintRowBackground(Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	if (rowIsSelected)
		g.fillAll(Colours::lightblue);
}

void PlaylistModel::paintCell(Graphics& g,
	int rowNumber,
	int columnId,
	int width, int height,
	bool /*rowIsSelected*/)
{
	g.setColour(Colours::black);

	if (columnId == 1) {
		g.drawText(String(rowNumber + 1), 2, 0, width - 4, height, Justification::centredLeft, true);
	}
	else if (columnId == 2) {
		g.drawText(m_playlist.getUnchecked(rowNumber).name, 2, 0, width - 4, height, Justification::centredLeft, true);
	}
	else if (columnId == 3) {
		g.drawText(Utils::formatSeconds(m_playlist.getUnchecked(rowNumber).durationInSeconds), 2, 0, width - 4, height, Justification::centredLeft, true);
	}

	g.setColour(Colours::black.withAlpha(0.2f));
	g.fillRect(width - 1, 0, 1, height);
}

Component* PlaylistModel::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
	jassert(existingComponentToUpdate == 0);
	return 0;
}

void PlaylistModel::add(String name, double durationInSeconds)
{
	PlaylistEntry entry;
	entry.name = name;
	entry.durationInSeconds = durationInSeconds;
	m_playlist.add(entry);
	sendChangeMessage();
}

void PlaylistModel::cellClicked(int rowNumber, int /*columnId*/, const MouseEvent & event)
{
	if (event.mods.isPopupMenu())
	{
		showPopup(rowNumber >= 0, rowNumber >= 0);
	}
}

void PlaylistModel::backgroundClicked(const MouseEvent & event)
{
	if (event.mods.isPopupMenu())
	{
		showPopup(false, false);
	}
}

void PlaylistModel::showPopup(bool enableInsert, bool enableDelete)
{

	PopupMenu popup;
	popup.addItem(1, "append");
	popup.addItem(2, "insert", enableInsert);
	popup.addItem(3, "delete", enableDelete);

	switch (popup.show()) {
	case 1:
		add("Test", 0);
		break;
	}
}

const Array<TrackConfig>& PlaylistModel::getTrackConfigs(int selectedRow)
{
	jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
	return m_playlist.getReference(selectedRow).trackConfigs;
}

void PlaylistModel::setTrackConfigs(int selectedRow, const Array<TrackConfig>& trackConfigs)
{
	jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
	PlaylistEntry entry = m_playlist.getUnchecked(selectedRow);
	entry.trackConfigs = trackConfigs;
	m_playlist.set(selectedRow, entry);
}

void PlaylistModel::setTrackDuration(int selectedRow, double duration)
{
	jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
	PlaylistEntry entry = m_playlist.getUnchecked(selectedRow);
	entry.durationInSeconds = duration;
	m_playlist.set(selectedRow, entry);
}

XmlElement* PlaylistModel::saveToXml(int row) const
{
	jassert(row >= 0 && row < m_playlist.size());
	return m_playlist.getUnchecked(row).saveToXml();
}

void PlaylistModel::addFromXml(const XmlElement& element)
{
	m_playlist.add(PlaylistEntry::createFromXml(element));
}

void PlaylistModel::clear()
{
	m_playlist.clear();
}