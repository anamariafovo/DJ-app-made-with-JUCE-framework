/*
  ==============================================================================

    DeckGUI.h

    ### Manage user interaction and waveform display ###

    - User interface for a single deck (one DJAudioPlayer)
	- Buttons: Play, Stop, Load, Library, Loop, Spectrogram/Waveform
    - Sliders: Volume, Speed (knob), Position
    - WaveformDisplay: shows track waveform
    - FileDragAndDropTarget: allows drag-and-drop loading
    - Timer: updates waveform playhead every 500 ms

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "MusicLibrary.h"
#include "WaveformDisplay.h"
#include "ButtonLookAndFeel.h"


class DeckGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public FileDragAndDropTarget,
    public Timer
{
    public:
        DeckGUI(DJAudioPlayer* player, AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse);
        ~DeckGUI();

        void paint(Graphics&) override;
        void resized() override;

		// Implement button listener
        void buttonClicked(Button* button) override;

		// Implement slider listener
        void sliderValueChanged(Slider* slider) override;

        bool isInterestedInFileDrag(const StringArray& files) override;
        void filesDropped(const StringArray& files, int x, int y) override;

        void timerCallback() override;

        // Function to open library
        void openLibraryWindow();

        void loadTrack(URL& url);

    private:
        FileChooser fChooser{ "Select a file..." };

        // Custom design for buttons
		ButtonLookAndFeel buttonDesign;

        // Labels
        Label volLabel;
        Label posLabel;
        Label speedLabel;

		// Buttons and Sliders
        TextButton playButton{ "PLAY" };
        TextButton stopButton{ "STOP" };
        TextButton loopButton{ "LOOP OFF" };
        TextButton loadButton{ "LOAD" };
        TextButton openLibraryButton{ "LIBRARY" }; // new button to open music library
		TextButton spectrogramButton{ "DISPLAY SPECTROGRAM" }; // new button to toggle spectrogram/waveform

        Slider speedKnob; // new rotary knob for speed control
        Slider volSlider;
        Slider posSlider;

        WaveformDisplay waveformDisplay;
        DJAudioPlayer* player;

		std::unique_ptr<juce::DialogWindow> libraryWindow; // pointer to dialog window

        bool looping = false; // new function for looping a song

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
