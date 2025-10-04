/*
  ==============================================================================

    MusicLibraryWindow.cpp

  ==============================================================================
*/

#include "MusicLibraryWindow.h"

MusicLibraryWindow::MusicLibraryWindow(MusicLibrary* library) : 
    DocumentWindow("Music Library",
    ColourPalette::bgColour,
    DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(library, true);
    centreWithSize(800, 600); // set initial size
    setResizable(true, true); // make the window resizable
    setVisible(true);
}

void MusicLibraryWindow::closeButtonPressed()
{
    setVisible(false);
}
