/*
    ==============================================================================

        MainComponent.cpp

    ==============================================================================
*/

#include "MainComponent.h"
#include "ColourPalette.h"

MainComponent::MainComponent()
{
    // Make sure you set the size of the component after you add any child components
    setSize(1000, 800); // set the initial size of the main component window

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
        && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio)) {
        // If audio recording permission is required and not granted, request it
        RuntimePermissions::request(RuntimePermissions::recordAudio, [&](bool granted) {
            if (granted)
                setAudioChannels(2, 2); // if granted, set up 2 input and 2 output channels
            });
    }
    else {
        // Specify the number of input and output channels that we want to open
        // Otherwise, set up 0 input and 2 output channels
        setAudioChannels(0, 2); 
    }

    addAndMakeVisible(deckGUI1); // add the first deck GUI component to the main component and make it visible
    addAndMakeVisible(deckGUI2); // add the second deck GUI component to the main component and make it visible

    formatManager.registerBasicFormats(); // register basic audio formats (e.g., WAV, MP3) with the format manager
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source
    shutdownAudio(); // release audio resources and shut down the audio device
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate); // prepare player1 for audio playback
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate); // prepare player2 for audio playback

    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate); // prepare the mixer source

    mixerSource.addInputSource(&player1, false); // add player1 as an input source to the mixer
    mixerSource.addInputSource(&player2, false); // add player2 as an input source to the mixer
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill); //fill the audio buffer with the next block from the mixer source
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being restarted due to a setting change

    // For more details, see the help for AudioProcessor::releaseResources()
    player1.releaseResources(); // release resources for player1
    player2.releaseResources(); // release resources for player2
    mixerSource.releaseResources(); // release resources for the mixer source
}

void MainComponent::paint(Graphics& g)
{
    g.fillAll(ColourPalette::bgColour); // fill the background
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    // Set the bounds of deckGUI1 to the left half of the window
    deckGUI1.setBounds(0, 0, getWidth() / 2, getHeight()); 
    // Set the bounds of deckGUI2 to the right half of the window
    deckGUI2.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight()); 
}
