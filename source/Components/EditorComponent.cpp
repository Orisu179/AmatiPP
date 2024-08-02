#include "EditorComponent.h"

EditorComponent::EditorComponent() : codeEditor (sourceCode, &tokeniser),
                                     buttons (juce::FlexBox::Direction::row, juce::FlexBox::Wrap::noWrap, juce::FlexBox::AlignContent::center, juce::FlexBox::AlignItems::flexStart, juce::FlexBox::JustifyContent::flexEnd),
                                     workDir (juce::File::getSpecialLocation (juce::File::userHomeDirectory)),
                                     font (juce::Font::getDefaultMonospacedFontName(), 18.0, juce::Font::plain)
{
    addAndMakeVisible (statusLabel);
    statusLabel.setText ("Status: Modified", juce::dontSendNotification);
    statusLabel.setFont (font);

    addAndMakeVisible (&codeEditor);
    compileButton.setComponentID ("compile");
    compileButton.setButtonText ("compile");
    compileButton.onClick = [this] {
        if (onCompile)
        {
            onCompile();
        }
    };
    addAndMakeVisible (compileButton);

    startButton.setComponentID ("start");
    startButton.setButtonText ("start");
    startButton.setEnabled (false);
    startButton.onClick = [this] {
        if (onStart && onStop)
        {
            if (startButton.getButtonText().equalsIgnoreCase ("start"))
            {
                onStart();
                startButton.setButtonText ("stop");
            }
            else
            {
                onStop();
                startButton.setButtonText ("start");
            }
        }
    };
    addAndMakeVisible (startButton);

    importButton.setButtonText ("Import");
    importButton.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser> ("Please select the DSP file you want to load..", workDir, "*.dsp");

        auto chooserFlag = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        fileChooser->launchAsync (chooserFlag, [this] (const juce::FileChooser& chooser) {
            auto sourceFile = chooser.getResult();
            setSource (sourceFile.loadFileAsString());
            workDir = sourceFile.getParentDirectory();
        });
    };
    addAndMakeVisible (importButton);
    exportButton.setButtonText ("export");
    exportButton.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser> ("Chose where to save DSP file", workDir, "*.dsp");
        auto chooserFlag = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;
        fileChooser->launchAsync (chooserFlag, [this] (const juce::FileChooser& chooser) {
            auto sourceFile = chooser.getResult();
            auto outputStream = juce::FileOutputStream (sourceFile);
            sourceCode.writeToStream (outputStream);
            workDir = sourceFile.getParentDirectory();
        });
    };
    addAndMakeVisible (exportButton);
    codeEditor.setFont (font);
}

void EditorComponent::resized()
{
    auto bounds = getLocalBounds();
    constexpr float margin = 10;
    const float buttonHeight = static_cast<float> (getHeight()) / 15; // 30
    const float buttonWidth = static_cast<float> (getWidth()) / 12; // 75
    constexpr float textHeight = 60; // 60
    constexpr float textWidth = 100; // 100

    auto addButton = [&] (auto& button) {
        button.changeWidthToFitText();
        buttons.items.add (juce::FlexItem (button).withMargin (margin).withMinHeight (buttonHeight).withMinWidth (buttonWidth).withMaxWidth (buttonWidth).withFlex (1));
    };
    addButton (compileButton);
    addButton (startButton);
    addButton (importButton);
    addButton (exportButton);

    buttons.performLayout (bounds.removeFromTop (buttonHeight));
    statusLabel.setBounds (getWidth() / 60, -5, textWidth, textHeight);

    codeEditor.setBounds (
        margin,
        2 * margin + buttonHeight,
        getWidth() - 2 * margin,
        getHeight() - 3 * margin - buttonHeight);
}

juce::String EditorComponent::getSource() const
{
    return sourceCode.getAllContent();
}

void EditorComponent::setSource (const juce::String& source)
{
    sourceCode.replaceAllContent (source);
}

void EditorComponent::setStatus (const juce::String& status, const juce::NotificationType notice)
{
    statusLabel.setText (status, notice);
}

void EditorComponent::setStartButtonEnabled (const bool state)
{
    startButton.setEnabled (state);
}
