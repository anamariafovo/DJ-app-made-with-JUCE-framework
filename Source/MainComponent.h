/*
  ==============================================================================

    MainComponent.h

    ### Initialize everything and mixes audio ###

    - The main GUI container that hosts two decks (DeckGUI) and the audio mixer
    - Handles audio device initialization and top-level audio flow

    - Creates DJAudioPlayer instances (player1, player2)
    - Creates DeckGUI instances (deckGUI1, deckGUI2) for UI controls
    - Uses a MixerAudioSource to mix audio from both decks
    - Registers basic audio formats using AudioFormatManager
    - Sets up input/output audio channels and handles permissions

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"

/*
    This component lives inside our window, and this is where you should put all
    your controls and content
*/
class MainComponent : public AudioAppComponent
{
    public:
        MainComponent();
        ~MainComponent();

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;

        void paint(Graphics& g) override;
        void resized() override;

    private:
        AudioFormatManager formatManager;
        AudioThumbnailCache thumbCache{ 100 };

        DJAudioPlayer player1{ formatManager };
        DJAudioPlayer player2{ formatManager };

        DeckGUI deckGUI1{ &player1, formatManager, thumbCache };
        DeckGUI deckGUI2{ &player2, formatManager, thumbCache };

        MixerAudioSource mixerSource;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
