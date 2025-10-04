/*
  ==============================================================================

    MusicLibraryWindow.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MusicLibrary.h"

class MusicLibraryWindow : public DocumentWindow
{
    public:
        MusicLibraryWindow(MusicLibrary* library);

        // Override close behaviour
        void closeButtonPressed() override;
};
