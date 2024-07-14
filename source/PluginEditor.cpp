#include "PluginEditor.h"
#include "PluginProcessor.h"

PluginEditor::PluginEditor (PluginProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p),
      processorRef (p),
      valueTreeState (vts),
      settingTree (vts.state.getOrCreateChildWithName ("settings", nullptr)),
      settingsComponent (settingTree),
      paramEditor (vts),
      tabbedComponent (juce::TabbedButtonBar::TabsAtTop)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    juce::LookAndFeel_V4::setDefaultLookAndFeel (&amatiLookAndFeel);
    setSize (800, 600);
    setResizable (true, true);
    setResizeLimits (800, 600, 1920, 1080);

    addAndMakeVisible (inspectButton);
    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    addAndMakeVisible (&tabbedComponent);

    juce::Colour tabColour = getLookAndFeel().findColour (juce::TabbedComponent::backgroundColourId);
    tabbedComponent.addTab ("Editor", tabColour, &editorComponent, false);
    tabbedComponent.addTab ("Parameters", tabColour, &paramEditor, false);
    tabbedComponent.addTab ("Console", tabColour, &consoleComponent, false);
    tabbedComponent.addTab ("Settings", tabColour, &settingsComponent, false);
    tabbedComponent.addTab ("Midi Keyboard", tabColour, &midiComponent, false);

    setResizable (true, true);
    setResizeLimits (100, 100, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());

    //---------------------------------------------------------------------------

    editorComponent.onCompile = [&] {
        consoleComponent.clearMessages();
        if (processorRef.compileSource (editorComponent.getSource()))
        {
            editorComponent.setStatus ("Status: Compiled", juce::sendNotification);
            processorRef.setPlayingState (false);
            updateParameters();
            editorComponent.setStartButtonEnabled (true);
        }
        else
        {
            editorComponent.setStatus ("Status: Error", juce::sendNotification);
            tabbedComponent.setCurrentTabIndex (2);
            editorComponent.setStartButtonEnabled (false);
        }
    };

    editorComponent.onStart = [&] {
        editorComponent.setStatus ("Status: Running", juce::sendNotification);
        processorRef.setPlayingState (true);
    };

    editorComponent.onStop = [&] {
        editorComponent.setStatus ("Status: Stopped", juce::sendNotification);
        processorRef.setPlayingState (false);
    };

    updateEditor(); // set editor to display the processor's source code
    updateParameters(); // set the right display for the parameters

    juce::Logger::setCurrentLogger (&consoleComponent);
    settingTree.addListener (this);
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel (nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    int margin = 10;
    auto bounds = getLocalBounds();

    inspectButton.setBounds (bounds.removeFromBottom (30));
    tabbedComponent.setBounds (bounds.reduced (margin, margin));
}

void PluginEditor::updateParameters()
{
    paramEditor.updateParameters (processorRef.getFaustParameter());
}

void PluginEditor::updateEditor()
{
    editorComponent.setSource (processorRef.getSourceCode());
}
