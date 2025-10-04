/*
  ==============================================================================

    MusicLibrary.cpp

  ==============================================================================
*/

#include "MusicLibrary.h"
#include "DeckGUI.h"
#include "BPMAnalyzer.h"
#include "ColourPalette.h"

MusicLibrary::MusicLibrary(DeckGUI* deckToLoadInto, BPMAnalyzer* bpmAnalyzer) : deck(deckToLoadInto), bpmAnalyzer(bpmAnalyzer)
{
    // Register basic formats
    formatManager.registerBasicFormats();

    // Add track button props
    addAndMakeVisible(addButton);
    addButton.addListener(this);
    addButton.setLookAndFeel(&buttonLookAndFeel);

    // Table setup
    addAndMakeVisible(table);
    table.setModel(this);
    table.getHeader().addColumn("Title", 1, 200);
    table.getHeader().addColumn("Duration", 2, 100);
    table.getHeader().addColumn("BPM", 3, 80);
    table.getHeader().addColumn("Artist", 4, 100);
    table.getHeader().addColumn("", 5, 80); // load button
    table.getHeader().addColumn("", 6, 80); // delete button
    table.getHeader().setColour(TableHeaderComponent::backgroundColourId, ColourPalette::bgColour);
    table.getHeader().setColour(TableHeaderComponent::textColourId, ColourPalette::textColour);
    table.getHeader().setColour(TableHeaderComponent::outlineColourId, ColourPalette::bgColour);
    table.setColour(ListBox::backgroundColourId, ColourPalette::bgColour);
    table.setColour(ListBox::outlineColourId, ColourPalette::btnColour);
    table.setOutlineThickness(1);
    table.setRowHeight(32);

    loadLibrary();
}

MusicLibrary::~MusicLibrary()
{

}

void MusicLibrary::paint(Graphics& g)
{
    g.fillAll(ColourPalette::bgColour);
}

void MusicLibrary::resized()
{
    auto area = getLocalBounds();
    addButton.setBounds(area.removeFromTop(40).reduced(5));
    table.setBounds(area.reduced(5));
}

void MusicLibrary::buttonClicked(Button* button)
{
    // Add button
    if (button == &addButton) {
        // Set flags to select one file
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;

        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                File chosenFile = chooser.getResult();
                if (chosenFile.exists()) {
                    addTrack(chosenFile); // add tracks to the library table
                }
            });
    }
}

int MusicLibrary::getNumRows()
{
    return tracks.size();
}

void MusicLibrary::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    // Change row colour if selected or not
    g.fillAll(rowIsSelected ? ColourPalette::textColour : ColourPalette::bgColour);
}

void  MusicLibrary::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(rowIsSelected ? ColourPalette::btnColour : ColourPalette::textColour);
    g.setFont(14.0f);
    // Title column with ellipsis in case it does not fit the width
    g.drawText(tracks[rowNumber].title, 2, 0, width - 4, height, Justification::centredLeft, true);
}

Component* MusicLibrary::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    // Duration column
    if (columnId == 2) {
        // Reuse component if it's already existent
        if (existingComponentToUpdate == nullptr) {
            auto* label = new Label();
            label->setColour(Label::textColourId, isRowSelected ? ColourPalette::btnColour : ColourPalette::textColour);
            label->setFont(14.0f);
            label->setJustificationType(Justification::centredLeft);
            int minutes = tracks[rowNumber].duration / 60; // minutes
            int seconds = tracks[rowNumber].duration % 60; // seconds
            // Format duration M:SS
            label->setText(String::formatted("%d:%02d", minutes, seconds), dontSendNotification);
            return label;
        }
        // Create a new one
        else {
            auto* label = (Label*)existingComponentToUpdate;
            label->setColour(Label::textColourId, isRowSelected ? ColourPalette::btnColour : ColourPalette::textColour);
            int minutes = tracks[rowNumber].duration / 60;
            int seconds = tracks[rowNumber].duration % 60;
            label->setText(String::formatted("%d:%02d", minutes, seconds), dontSendNotification);
            return label;
        }
    }
    // BPM column
    else if (columnId == 3) {
        if (existingComponentToUpdate == nullptr) {
            auto* label = new Label();
            label->setColour(Label::textColourId, isRowSelected ? ColourPalette::btnColour : ColourPalette::textColour);
            label->setFont(14.0f);
            label->setJustificationType(Justification::centredLeft);
            String bpmStr = tracks[rowNumber].bpm > 0 ? String((int)tracks[rowNumber].bpm) : "--";
            label->setText(bpmStr, dontSendNotification);
            return label;
        }
        else {
            auto* label = (Label*)existingComponentToUpdate;
            label->setColour(Label::textColourId, isRowSelected ? ColourPalette::btnColour : ColourPalette::textColour);
            String bpmStr = tracks[rowNumber].bpm > 0 ? String((int)tracks[rowNumber].bpm) : "--";
            label->setText(bpmStr, dontSendNotification);
            return label;
        }
    }
    // Artist column
    else if (columnId == 4) {
        if (existingComponentToUpdate == nullptr) {
            auto* label = new Label();
            label->setColour(Label::textColourId, isRowSelected ? ColourPalette::btnColour : ColourPalette::textColour);
            label->setFont(14.0f);
            label->setJustificationType(Justification::centredLeft);
            label->setText(tracks[rowNumber].artist, dontSendNotification);
            return label;
        }
        else {
            auto* label = (Label*)existingComponentToUpdate;
            label->setColour(Label::textColourId, isRowSelected ? ColourPalette::btnColour : ColourPalette::textColour);
            label->setText(tracks[rowNumber].artist, dontSendNotification);
            return label;
        }
    }
    // Load track button column
    else if (columnId == 5) {
        TextButton* btn = (TextButton*)existingComponentToUpdate;
        if (btn == nullptr) {
            auto area = getLocalBounds();
            btn = new TextButton("Load");
            btn->setLookAndFeel(&buttonLookAndFeel);
            btn->onClick = [this, rowNumber]() {
                if (deck != nullptr) {
                    deck->loadTrack(tracks[rowNumber].fileURL);
                }
            };
        }
        return btn;
    }
    // Delete track
    else if (columnId == 6)
    {
        TextButton* btn = (TextButton*)existingComponentToUpdate;
        if (btn == nullptr)
        {
            btn = new TextButton("Delete");
            btn->setLookAndFeel(&buttonLookAndFeel);
            btn->onClick = [this, rowNumber]() {
                if (rowNumber >= 0 && rowNumber < tracks.size()) {
                    tracks.remove(rowNumber);
                    saveLibrary();
                    table.updateContent(); // reassign row numbers
                }
            };
        }
        return btn;
    }


    return nullptr;
}

void MusicLibrary::addTrack(const File& f)
{
    // Add metadata to a track
    Track t;
    t.title = f.getFileName();
    t.duration = 0;
    t.artist = "Unknown Artist";
    t.fileURL = URL{ f };
    t.bpm = bpmAnalyzer->estimateBPM(f);

    if (auto* reader = formatManager.createReaderFor(f)) {
        // Read the duration and convert to int representing seconds
        t.duration = static_cast<int>(reader->lengthInSamples / reader->sampleRate);

        auto metadata = reader->metadataValues;

        if (metadata.containsKey("artist"))
            t.artist = metadata["artist"]; 

        // Check also for standard ID3 tag that contains artist name
        else if (metadata.containsKey("ID3:TPE1"))
            t.artist = metadata["ID3:TPE1"];

        delete reader;
    }

    tracks.add(t);
    table.updateContent();
    // Save to JSON file
    saveLibrary();
}

void MusicLibrary::saveLibrary()
{
    var libraryJson;
    // Iterate through the tracks array
    for (auto& t : tracks)
    {
        // Create new object to save to the JSON
        DynamicObject* obj = new DynamicObject();
        obj->setProperty("title", t.title);
        obj->setProperty("duration", t.duration);
        obj->setProperty("artist", t.artist);
        obj->setProperty("url", t.fileURL.toString(false));
        obj->setProperty("bpm", t.bpm);
        libraryJson.append(var(obj));
    }
    // Find the file or create one
    File f = File::getCurrentWorkingDirectory().getChildFile("library.json");
    // Save data from object to the file
    String jsonString = JSON::toString(libraryJson);
    f.replaceWithText(jsonString);
}

void MusicLibrary::loadLibrary()
{
    // Clear tracks array to add all from JSON file again on startup
    tracks.clear();
    File f = File::getCurrentWorkingDirectory().getChildFile("library.json");

    if (f.existsAsFile())
    {
        var library = JSON::parse(f);

        if (library.isArray())
        {
            // Iterate through all items
            for (auto& item : *library.getArray())
            {
                // Create obj variable to hold item's data and check if it is ok
                if (auto* obj = item.getDynamicObject())
                {
                    // Create track and add to the tracks array 
                    Track t;
                    t.title = obj->getProperty("title").toString();
                    t.duration = (int)obj->getProperty("duration");
                    t.artist = obj->getProperty("artist");
                    t.fileURL = URL(obj->getProperty("url").toString());
                    t.bpm = obj->getProperty("bpm");
                    tracks.add(t);
                }
            }
        }
    }
    table.updateContent();
}

URL MusicLibrary::getTrackURL(int row)
{
    if (row >= 0 && row < tracks.size())
        return tracks[row].fileURL;
    return {};
}
