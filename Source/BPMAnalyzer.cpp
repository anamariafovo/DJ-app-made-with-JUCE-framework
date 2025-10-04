/*
  ==============================================================================

    BPMAnalyzer.cpp

  ==============================================================================
*/

#include "BPMAnalyzer.h"

double BPMAnalyzer::estimateBPM(const File& file)
{
	// Set up formatManager
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

	// Create reader to read samples from the audio file
    if (auto* reader = formatManager.createReaderFor(file)) {
		// Read audio samples into buffer
        int numChannels = (int)reader->numChannels; // number of channels
        int numSamples = (int)reader->lengthInSamples; // number of samples in the file
        AudioBuffer<float> buffer(numChannels, numSamples); // buffer to hold the audio data for all channels
		reader->read(&buffer, 0, numSamples, 0, true, true); // read data

        // Convert to mono to average all channels into one channel
        AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
			monoBuffer.addFrom(0, 0, buffer, ch, 0, buffer.getNumSamples(), 1.0f / buffer.getNumChannels());
        }

		// Simple peak detection for BPM estimation
		int sampleRate = (int)reader->sampleRate; // sample rate
		int numSamplesMono = monoBuffer.getNumSamples(); // number of samples
        float threshold = 0.5f * monoBuffer.getMagnitude(0, numSamplesMono); // magnitude threshold
		int beatCount = 0; // count of detected beats
		int lastSample = -10000; // last detected beat sample index

        // Check if sample is above threshold
        for (int i = 0; i < numSamplesMono; ++i)
        {
            // Add 0.25s min distance to avoid counting the same beat
            if (fabs(monoBuffer.getSample(0, i)) > threshold && i - lastSample > sampleRate / 4) 
            {
                beatCount++;
                lastSample = i;
            }
        }

		// Calculate BPM
        double durationSec = numSamplesMono / (double)sampleRate; // song's duration in sec
        double bpm = (beatCount / durationSec) * 60.0; // beats per minute

        delete reader;
        return bpm;
    }

	// Failed to read file
    return 0.0;
}
