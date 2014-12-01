#include "PlaylistModel.h"

#include "Utils.h"


class PlayNextButton : public ImageButton
{
public:
	void setRowNumber(int rowNumber) { m_rowNumber = rowNumber; }
	int getRowNumber() { return m_rowNumber; }

private:
	int m_rowNumber;
};

XmlElement* PlaylistEntry::saveToXml() const
{
	XmlElement* entryXml = new XmlElement("Entry");
	
	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(name);
	entryXml->addChildElement(nameXml);

	entryXml->setAttribute("playNext", playNext);

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
	entry.playNext = element.getBoolAttribute("playNext");

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
	if (columnId == 4) {
		// If it's the output channel column, we'll return our custom component..

		PlayNextButton* editor = static_cast<PlayNextButton*>(existingComponentToUpdate);

		// If an existing component is being passed-in for updating, we'll re-use it, but
		// if not, we'll have to create one.
		if (editor == nullptr) {
			editor = new PlayNextButton();
			Image normalImage = ImageFileFormat::loadFrom(BinaryData::arrowrightdouble_png, BinaryData::arrowrightdouble_pngSize);
			Image desaturedImage(normalImage.createCopy());
			desaturedImage.desaturate();
			editor->setClickingTogglesState(true);
			editor->setImages(true, true, true,
				desaturedImage, 0.7f, Colours::transparentBlack,
				normalImage, 1.0f, Colours::transparentBlack,
				normalImage, 1.0f, Colours::transparentBlack,
				0.0f);
			editor->addListener(this);
		}

		bool playNext = m_playlist.getUnchecked(rowNumber).playNext;
		editor->setToggleState(playNext, sendNotification);
		editor->setRowNumber(rowNumber);

		return editor;
	}
	else {
		// for any other column, just return 0, as we'll be painting these columns directly.

		jassert(existingComponentToUpdate == 0);
		return 0;
	}
}

void PlaylistModel::buttonClicked(Button* button)
{
	int rowNumber = static_cast<PlayNextButton*>(button)->getRowNumber();

	m_playlist.getReference(rowNumber).playNext = button->getToggleState();
}

void PlaylistModel::add(String name, double durationInSeconds)
{
	PlaylistEntry entry;
	entry.name = name;
	entry.durationInSeconds = durationInSeconds;
	entry.playNext = false;
	m_playlist.add(entry);
	sendChangeMessage();
}

void PlaylistModel::insert(int rowNumber, String name, double durationInSeconds)
{
	PlaylistEntry entry;
	entry.name = name;
	entry.durationInSeconds = durationInSeconds;
	entry.playNext = false;
	m_playlist.insert(rowNumber, entry);
	sendChangeMessage();
}

void PlaylistModel::cellClicked(int rowNumber, int /*columnId*/, const MouseEvent & event)
{
	if (event.mods.isPopupMenu())
	{
		showPopup(rowNumber, rowNumber >= 0, rowNumber >= 0);
	}
}

void PlaylistModel::backgroundClicked(const MouseEvent & event)
{
	if (event.mods.isPopupMenu())
	{
		showPopup(-1, false, false);
	}
}

void PlaylistModel::showPopup(int rowNumber, bool enableInsert, bool enableDelete)
{

	PopupMenu popup;
	popup.addItem(1, "append");
	popup.addItem(2, "insert", enableInsert);
	popup.addItem(3, "edit", enableInsert);
	popup.addItem(4, "delete", enableDelete);

	switch (popup.show()) {
	case 1:
		add("", 0);
		break;
	case 2:
		insert(rowNumber, "", 0);
		break;
	case 3:
	{
		PlaylistEntrySettingsChangedCallback callback = [this, rowNumber](String name) {
			PlaylistEntry entry = m_playlist.getUnchecked(rowNumber);
			entry.name = name;
			m_playlist.set(rowNumber, entry);
			sendChangeMessage();
		};
		m_editDialog = ScopedPointer<PlaylistEntryDialogWindow>(new PlaylistEntryDialogWindow(m_playlist.getUnchecked(rowNumber).name, callback));
		break;
	}
	case 4:
		remove(rowNumber);
		break;
	}
}

void PlaylistModel::remove(int rowNumber)
{
	m_playlist.remove(rowNumber);
	sendChangeMessage();

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

bool PlaylistModel::doPlayNext(int selectedRow)
{
	return m_playlist.getUnchecked(selectedRow).playNext;
}