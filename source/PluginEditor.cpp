#include "PluginEditor.h"
#include "PluginProcessor.h"

PluginEditor::PluginEditor (PluginProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p),
      processorRef (p),
      valueTreeState (vts),
      settingTree (vts.state.getOrCreateChildWithName ("settings", nullptr)),
      tabbedComponent (juce::TabbedButtonBar::TabsAtTop),
      settingsComponent (settingTree),
      paramEditor (vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    juce::LookAndFeel_V4::setDefaultLookAndFeel (&amatiLookAndFeel);
    setResizable (true, false);
    setResizeLimits (800, 600, 1920, 1080);
    setSize (800, 600);

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

    const juce::Colour tabColour = getLookAndFeel().findColour (juce::TabbedComponent::backgroundColourId);
    paramViewport.setViewedComponent (&paramEditor, false);
    paramViewport.setScrollBarsShown (true, false);
    tabbedComponent.addTab ("Editor", tabColour, &editorComponent, false);
    tabbedComponent.addTab ("Parameters", tabColour, &paramViewport, false);
    tabbedComponent.addTab ("Console", tabColour, &consoleComponent, false);
    tabbedComponent.addTab ("Midi Keyboard", tabColour, &midiComponent, false);
    tabbedComponent.addTab ("Settings", tabColour, &settingsComponent, false);

    setResizable (true, true);
    setResizeLimits (100, 100, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());

    //---------------------------------------------------------------------------

    editorComponent.onCompile = [&] {
        onCompile();
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
    constexpr int margin = 10;
    const int parameterHeight = static_cast<int>(processorRef.getFaustParameter().size() * 75);
    auto bounds = getLocalBounds();

    inspectButton.setBounds (bounds.removeFromBottom (30));
    tabbedComponent.setBounds (bounds.reduced (margin, margin));
    if( parameterHeight > tabbedComponent.getHeight())
    {
        const int addedHeight = parameterHeight - tabbedComponent.getHeight();
        paramViewport.setBounds (tabbedComponent.getLocalBounds().expanded (0, addedHeight));
    } else
    {
        paramViewport.setBounds (tabbedComponent.getLocalBounds());
    }
    paramEditor.setBounds (paramViewport.getLocalBounds());
}

void PluginEditor::updateParameters()
{
    paramEditor.updateParameters (processorRef.getFaustParameter());
}

void PluginEditor::updateEditor()
{
    editorComponent.setSource (processorRef.getSourceCode());
}

void PluginEditor::onCompile()
{
    consoleComponent.clearMessages();
    if (processorRef.compileSource (editorComponent.getSource()))
    {
        editorComponent.setStatus ("Status: Compiled", juce::sendNotification);
        processorRef.setPlayingState (false);
        updateParameters();
        editorComponent.setStartButtonEnabled (true);
        midiComponent.setHandleMidiHandler ([&](const juce::MidiMessage& message) {
          processorRef.handleMidi (message);
        });
        tabbedComponent.setCurrentTabIndex(0);
    }
    else
    {
        editorComponent.setStatus ("Status: Error", juce::sendNotification);
        tabbedComponent.setCurrentTabIndex (2);
        editorComponent.setStartButtonEnabled (false);
    }
}
