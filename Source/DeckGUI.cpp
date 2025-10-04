/*
  ==============================================================================

    DeckGUI.cpp

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"
#include "MusicLibraryWindow.h"
#include "ColourPalette.h"

DeckGUI::DeckGUI(
    DJAudioPlayer* _player,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse
) :
    player(_player),
    waveformDisplay(formatManagerToUse, cacheToUse)
{
    // Play button
    addAndMakeVisible(playButton);
    playButton.addListener(this);
    playButton.setLookAndFeel(&buttonDesign);

	// Stop button
    addAndMakeVisible(stopButton);
    stopButton.addListener(this);
    stopButton.setLookAndFeel(&buttonDesign);

	// Load button
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    loadButton.setLookAndFeel(&buttonDesign);

    // Loop button
    addAndMakeVisible(loopButton);
    loopButton.addListener(this);
    loopButton.setLookAndFeel(&buttonDesign);

    // Library button
    addAndMakeVisible(openLibraryButton);
    openLibraryButton.setLookAndFeel(&buttonDesign);
    openLibraryButton.onClick = [this] { openLibraryWindow(); };

    // Spectrogram button
    addAndMakeVisible(spectrogramButton);
    spectrogramButton.addListener(this);
    spectrogramButton.setLookAndFeel(&buttonDesign);

    // Volume slider
    addAndMakeVisible(volSlider);
    volSlider.addListener(this);
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(1.0);
    volSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    volSlider.setColour(Slider::thumbColourId, ColourPalette::tertiaryColour);
    addAndMakeVisible(volLabel);
    volLabel.setText("VOL", dontSendNotification);
    volLabel.setColour(Label::textColourId, ColourPalette::textColour);
    volLabel.setJustificationType(Justification::centred);

	// Position slider
    addAndMakeVisible(posSlider);
    posSlider.addListener(this);
    posSlider.setRange(0.0, 1.0);
    posSlider.setValue(0.0);
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    posSlider.setColour(Slider::thumbColourId, ColourPalette::tertiaryColour);
    addAndMakeVisible(posLabel);
    posLabel.setText("POS", dontSendNotification);
    posLabel.setColour(Label::textColourId, ColourPalette::textColour);
    posLabel.setJustificationType(Justification::centred);

	// Speed knob
    addAndMakeVisible(speedKnob);
    speedKnob.addListener(this);
    speedKnob.setSliderStyle(Slider::Rotary);
    speedKnob.setRange(0.0, 2.0);
    speedKnob.setValue(1.0); // default value fro speed knob set to 1 = normal speed
    speedKnob.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    speedKnob.setColour(Slider::thumbColourId, ColourPalette::secondaryColour);
    addAndMakeVisible(speedLabel);
    speedLabel.setText("Speed", dontSendNotification);
    speedLabel.setColour(Label::textColourId, ColourPalette::textColour);
    speedLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(waveformDisplay);

    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    playButton.setLookAndFeel(nullptr);
    stopButton.setLookAndFeel(nullptr);
    loadButton.setLookAndFeel(nullptr);
    loopButton.setLookAndFeel(nullptr);
    openLibraryButton.setLookAndFeel(nullptr);
    loopButton.removeListener(this);
    speedKnob.removeListener(this);
    stopTimer();
}

void DeckGUI::paint(Graphics& g)
{
	g.fillAll(ColourPalette::bgColour); // background color
	g.setColour(ColourPalette::textColour); // text color
	g.setFont(14.0f); // font size
}

void DeckGUI::resized()
{
    int padding = 4;
	double rowH = getHeight() / 7; // divide height into 7 rows
	int buttonWidth = (getWidth() - padding * 4) / 3; // divide width into 3 columns with padding
    int buttonHeight = rowH - padding * 2; // set height with padding

	// First section - LOAD, LIBRARY, SPECTROGRAM buttons
    loadButton.setBounds(padding, padding, buttonWidth, buttonHeight);
    openLibraryButton.setBounds(padding * 2 + buttonWidth, padding, buttonWidth, buttonHeight);
    spectrogramButton.setBounds(padding * 3 + buttonWidth * 2, padding, buttonWidth, buttonHeight);

	// Second section - WAVEFORM/SPECTROGRAM display (3 rows high)
    waveformDisplay.setBounds(padding, rowH + padding, getWidth() - padding * 2, rowH * 3 - padding);

	// Third section - SPEED knob, VOLUME and POSITION sliders
    speedKnob.setBounds(1, rowH * 4.5, getWidth() / 3, rowH);
    speedLabel.setBounds(speedKnob.getX(), rowH * 5.25, getWidth() / 3, rowH / 2);

    volSlider.setBounds(getWidth() / 3, rowH * 4, getWidth() - speedKnob.getWidth() - 10, rowH);
    volLabel.setBounds(getWidth() / 3, volSlider.getY(), volSlider.getWidth(), rowH / 2);

    posSlider.setBounds(getWidth() / 3, rowH * 5, getWidth() - speedKnob.getWidth() - 10, rowH);
    posLabel.setBounds(getWidth() / 3, posSlider.getY(), posSlider.getWidth(), rowH / 2);

	// Fourth section - PLAY, STOP, LOOP buttons
    playButton.setBounds(padding, rowH * 6 + padding, buttonWidth, buttonHeight);
    stopButton.setBounds(padding * 2 + buttonWidth, rowH * 6 + padding, buttonWidth, buttonHeight);
    loopButton.setBounds(padding * 3 + buttonWidth * 2, rowH * 6 + padding, buttonWidth, buttonHeight);
}

void DeckGUI::buttonClicked(Button* button)
{
	// Play button functionality
    if (button == &playButton) {
        player->start();
    }
	// Stop button functionality
    if (button == &stopButton) {
        player->stop();
    }
	// Load button functionality
    if (button == &loadButton) {
        // Set flag to select single files
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;

        fChooser.launchAsync(fileChooserFlags, [player = player, &waveform = waveformDisplay](const FileChooser& chooser)
            {
                File chosenFile = chooser.getResult();
                if (chosenFile.exists()) {
                    URL url{ chosenFile };
                    player->loadURL(url);
                    waveform.loadURL(url);
                }
            });

    }
	// Loop button functionality
    if (button == &loopButton) {
        looping = !looping;
        // Set loop true or false in DJAudioPlayer
        player->setLooping(looping); 
        loopButton.setButtonText(looping ? "LOOP ON" : "LOOP OFF");
        repaint();
    }
	// Spectrogram button functionality
    if (button == &spectrogramButton) {
        // Toggle isSectrogramEnabled
        bool enabled = !waveformDisplay.getSpectrogramEnabled();
        waveformDisplay.setSpectrogramEnabled(enabled);
        if (enabled && player != nullptr) {
            waveformDisplay.generateSpectrogram(player->getURL());
        }
        spectrogramButton.setButtonText(enabled ? "DISPLAY WAVE" : "DISPLAY SPECTROGRAM");
    }
}

void DeckGUI::sliderValueChanged(Slider* slider)
{
	// Volume functionality
    if (slider == &volSlider) {
        player->setGain(slider->getValue());
    }
	// Position functionality
    if (slider == &posSlider) {
        player->setPositionRelative(slider->getValue());
    }
	// Speed functionality
    if (slider == &speedKnob) {
        player->setSpeed(slider->getValue()); // set speed from knob
    }
}

bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    if (files.size() >= 1) {
        URL fileUrl = URL{ File{files[0]} };
        player->loadURL(fileUrl);
        waveformDisplay.loadURL(fileUrl);
    }
}

void DeckGUI::timerCallback()
{
    double pos = player->getPositionRelative();
    waveformDisplay.setPositionRelative(pos);
    posSlider.setValue(pos, dontSendNotification);
}

void DeckGUI::openLibraryWindow()
{
    // Add 'this' as argument to the library so it knows to which DeckGUI to load the track
    new MusicLibraryWindow(new MusicLibrary(this));
}

void DeckGUI::loadTrack(URL& url)
{
    if (player != nullptr) {
        player->loadURL(url);
        waveformDisplay.loadURL(url);
    }
}
