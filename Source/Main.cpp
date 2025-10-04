/*
  ==============================================================================

    Main.cpp

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

class OtoDecksApplication  : public JUCEApplication
{
public:
    OtoDecksApplication() {}
    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise (const String& commandLine) override
    {
        // App's initialisation code
        mainWindow.reset (new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        // App's shutdown code here
        mainWindow = nullptr; // deletes the window
    }

    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit
        quit();
    }

    void anotherInstanceStarted(const String& commandLine) override
    {
        /* When another instance of the app is launched while this one is running,
        this method is invoked, and the commandLine parameter tells you what
        the other instance's command-line arguments were. */
    }

    /*
        This class implements the desktop window that contains 
        an instance of our MainComponent class
    */
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name) : DocumentWindow(
            name,
            Desktop::getInstance().getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
            DocumentWindow::allButtons
        ) {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
           #else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
           #endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window.
            // Here, we'll just ask the app to quit when this happens, but you can change this to do whatever you need
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

// This macro generates the main() routine that launches the app
START_JUCE_APPLICATION(OtoDecksApplication)
