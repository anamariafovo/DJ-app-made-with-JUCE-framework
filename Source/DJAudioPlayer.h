/*
  ==============================================================================

    DJAudioPlayer.h

    ### Handle audio playback logic ###

    - An individual audio player with playback controls
    - Allows setting gain, playback speed, and position
    - Loads audio from a URL
    - ResamplingAudioSource to control playback speed
    - getPositionRelative() to track playhead progress

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public AudioSource 
{
    public:
        DJAudioPlayer(AudioFormatManager& _formatManager);
        ~DJAudioPlayer();

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;

        void loadURL(URL audioURL);

        void setGain(double gain);
        void setSpeed(double ratio);
        void setPosition(double posInSecs);
        double getPositionRelative();
        void setPositionRelative(double pos);

        void start();
        void stop();

        // Loop track functionality
        void setLooping(bool loopTrack);
        bool getLooping() const;
        URL getURL() const;

    private:
        AudioFormatManager& formatManager;
        AudioTransportSource transportSource;
        ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
        std::unique_ptr<AudioFormatReaderSource> readerSource;

        bool looping = false;
        URL currentURL;
};
