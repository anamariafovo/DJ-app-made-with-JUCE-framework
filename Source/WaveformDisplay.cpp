/*
  ==============================================================================

    WaveformDisplay.cpp

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"
#include "DeckGUI.h"
#include "ColourPalette.h"

WaveformDisplay::WaveformDisplay(
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse
) :
    audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0)
{
    audioThumb.addChangeListener(this);

    // FFT object 
    fft = std::make_unique<dsp::FFT>(fftOrder);
    // Hann window to prevent leakage and keep amplitude accuracy
    window = std::make_unique<dsp::WindowingFunction<float>>(fftSize, dsp::WindowingFunction<float>::hann);
}

WaveformDisplay::~WaveformDisplay()
{

}

void WaveformDisplay::paint(Graphics& g)
{
    g.fillAll(ColourPalette::btnColour);
    g.setColour(ColourPalette::bgColour); // set colour for separator line
    g.drawLine(0.0f, 0.0f, getWidth(), 0.0f, 1.0f); // separator line
    g.setColour(ColourPalette::accentColour);

    if (fileLoaded) {
        if (isSpectrogramEnabled && spectrogramImage.isValid()) {
            // Draw spectogram
            g.drawImageWithin(spectrogramImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::fillDestination);
        }
        else {
            // Draw wave
            audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
        }
        g.setColour(ColourPalette::tertiaryColour);
        g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight());
    }
    else {
        g.setFont(20.0f);
        g.drawText("File not loaded", getLocalBounds(), Justification::centred, true);
    }
}

void WaveformDisplay::resized()
{

}

void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));

    if (fileLoaded) {
        if (isSpectrogramEnabled)
            generateSpectrogram(audioURL);
        repaint();
    }
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    repaint();
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position) {
        position = pos;
        repaint();
    }
}

void WaveformDisplay::generateSpectrogram(URL audioURL)
{
    // Create empty spectrogram image
    spectrogramImage = Image(Image::RGB, getWidth(), getHeight(), true);
    spectrogramImage.clear(spectrogramImage.getBounds(), ColourPalette::btnColour);

    // Open file as input stream
    auto input = audioURL.createInputStream(false);
    if (!input) return;

    // Configure formatManager to read audio data
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    auto* reader = formatManager.createReaderFor(std::move(input)); // create reader
    if (!reader) return;

    int numChannels = (int)reader->numChannels; // number of channels
    int numSamples = (int)reader->lengthInSamples; // number of samples in the file
    AudioBuffer<float> buffer(numChannels, numSamples); // buffer to hold the audio data for all channels
    reader->read(&buffer, 0, numSamples, 0, true, true); // read the entire audio file into the buffer

    // Process each vertical column of pixels in the image
    for (int x = 0; x < getWidth(); ++x)
    {
        // Calculate the start sample index for this slice
        int startSample = (int)((x / (float)getWidth()) * (numSamples - fftSize));
        if (startSample + fftSize >= numSamples) break; // stop if samples are finished

        // Fill FFT input data with samples
        for (int i = 0; i < fftSize; ++i)
        {
            float sample = 0.0f;
            // Sum samples from all channels in case of stereo file
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
                sample += buffer.getSample(ch, startSample + i);
            }
            fftData[i] = sample / buffer.getNumChannels(); // average
        }

        window->multiplyWithWindowingTable(fftData.data(), fftSize);
        fft->performFrequencyOnlyForwardTransform(fftData.data());

        // Loop over each pixel in the column
        for (int y = 0; y < getHeight(); ++y)
        {
            int fftBin = jmap(y, 0, getHeight() - 1, 0, fftSize / 2 - 1);
            float magnitude = fftData[fftBin]; // get magnitude of this frequency bin

            float dB = Decibels::gainToDecibels(magnitude, -100.0f); // convert the raw magnitute to decibels
            float level = jmap(dB, -100.0f, 0.0f, 0.0f, 1.0f); // normalize to range 0.0 - 1.0

            // Set the pixel colour based on level/magnitute
            Colour colour = getSpectrogramColour(level);
            // Flip y axis so low frequencies are at the bottom
            spectrogramImage.setPixelAt(x, getHeight() - 1 - y, colour);
        }
    }

    repaint();
}


void WaveformDisplay::setSpectrogramEnabled(bool enabled) 
{
    isSpectrogramEnabled = enabled;
    repaint();
}

bool WaveformDisplay::getSpectrogramEnabled() const 
{
    return isSpectrogramEnabled;
}

Colour WaveformDisplay::getSpectrogramColour(float level)
{
    // Start from lower magnitute
    if (level < 0.2f)
        return ColourPalette::btnColour;
    if (level < 0.4f)
        return ColourPalette::primaryColour;
    if (level < 0.6f)
        return ColourPalette::tertiaryColour;
    if (level < 0.8f)
        return ColourPalette::secondaryColour;
    // Set yellow as default for all the higher levels
    return ColourPalette::accentColour;
}
