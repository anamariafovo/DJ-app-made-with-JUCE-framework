/*
  ==============================================================================

    BPMAnalyzer.h

    - Estimate BPM from each uploaded track in the library
    - Uses peak detection to find spikes in the volume (drum hit)

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class BPMAnalyzer {
    public:
        double estimateBPM(const File& audioFile);
};
