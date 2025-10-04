/*
  ==============================================================================

    WaveformDisplay.h

    ### Show the audio visually ###

    - Visualizes the waveform of an audio track
    - AudioThumbnail to render waveforms
    - setPositionRelative moves the playhead indicator

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class WaveformDisplay : public Component, public ChangeListener
{
    public:
        WaveformDisplay(AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse);
        ~WaveformDisplay();

        void paint(Graphics&) override;
        void resized() override;

        void changeListenerCallback(ChangeBroadcaster* source) override;

        void loadURL(URL audioURL);

        // Set the relative position of the playhead
        void setPositionRelative(double pos);

        // Spectrogram related
        void generateSpectrogram(URL audioURL); // function to generate spectogram image
        void setSpectrogramEnabled(bool enabled); // setter
        bool getSpectrogramEnabled() const; // getter
        Colour getSpectrogramColour(float level); // find colour

    private:
        AudioThumbnail audioThumb;
        bool fileLoaded;
        double position;

        // Spectrogram related
        bool isSpectrogramEnabled = false;
        Image spectrogramImage;
        enum { fftOrder = 10, fftSize = 1 << fftOrder }; 
        std::unique_ptr<dsp::FFT> fft; // FFT object for frequency analysis
        std::unique_ptr<dsp::WindowingFunction<float>> window;
        std::vector<float> fftData = std::vector<float>(fftSize * 2, 0.0f); // buffer to hold FFT data

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
