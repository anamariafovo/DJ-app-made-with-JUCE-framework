/*
  ==============================================================================

	MusicLibrary.h

	- Add multiple audio tracks
	- Display each track's details (title, artist, duration)
	- Load tracks from library into decks
	- Persistent memory

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "ButtonLookAndFeel.h"

struct Track {
	String title;
	int duration;
	String artist;
	URL fileURL;
	double bpm = 0.0;
};

class DeckGUI; // forward declaration
class BPMAnalyzer; // forward declaration

class MusicLibrary : public Component,
	public TableListBoxModel,
	public Button::Listener
{
	public:
		MusicLibrary(DeckGUI* deckToLoadInto = nullptr, BPMAnalyzer* bpmAnalyzer = nullptr);
		~MusicLibrary() override;

		void paint(Graphics&) override;
		void resized() override;
		void buttonClicked(Button* button) override;

		// Methods to control the table list box display
		int getNumRows() override;
		void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
		void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
		Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

		// Library controls - add, save, load library
		void addTrack(const File& f);
		void saveLibrary();
		void loadLibrary();

		URL getTrackURL(int row); // get track URL by row

	private:
		FileChooser fChooser{ "Select a file..." };

		DeckGUI* deck = nullptr; // pointer to the deck to load tracks into
		BPMAnalyzer* bpmAnalyzer = nullptr; // pointer to the bpm analyzer

		AudioFormatManager formatManager;

		ButtonLookAndFeel buttonLookAndFeel; // custom button design
		TextButton addButton{ "Add Tracks" };

		TableListBox table; // table that contains all tracks with details
		Array<Track> tracks; // array to handle saved tracks

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicLibrary)
};
