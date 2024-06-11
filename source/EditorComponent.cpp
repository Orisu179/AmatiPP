#include "EditorComponent.h"
EditorComponent::EditorComponent() : codeEditor (sourceCode, &tokeniser),
                                     workDir (juce::File::getSpecialLocation (juce::File::userHomeDirectory))
{
    addAndMakeVisible (statusLabel);
    statusLabel.setText ("Status: Modified", juce::dontSendNotification);

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
    revertButton.setButtonText ("Revert");
    addAndMakeVisible (revertButton);

    //==========
    auto font = juce::Font (juce::Font::getDefaultMonospacedFontName(), 18.0, juce::Font::plain);
    codeEditor.setFont (font);
}

void EditorComponent::resized()
{
    using FB = juce::FlexBox;
    int margin = 10;
    int buttonHeight = 30;
    int buttonWidth = 75;
    int textHeight = 40;
    int textWidth = 100;
    buttons.flexDirection = FB::Direction::row;
    buttons.alignItems = FB::AlignItems::flexStart;
    buttons.flexWrap = FB::Wrap::noWrap;
    buttons.justifyContent = FB::JustifyContent::flexEnd;

    topBar.flexWrap = FB::Wrap::noWrap;
    topBar.flexDirection = FB::Direction::row;
    topBar.alignItems = FB::AlignItems::flexStart;
    topBar.justifyContent = FB::JustifyContent::flexStart;

//    auto addButton = [&] (auto& button) {
//        buttons.items.add (juce::FlexItem(button).
//                           withMargin(margin).
//                           withMinHeight(buttonHeight).
//                           withMaxWidth(buttonWidth).
//                           withFlex(1.0));
//    };
//    addButton (compileButton);
//    addButton (importButton);
//    addButton (exportButton);
//    addButton (revertButton);

    topBar.items.add(juce::FlexItem(statusLabel).withMinHeight(textHeight).withMaxWidth(textWidth).withFlex(1.0));
//    topBar.items.add(juce::FlexItem(buttons).withFlex(2.0));

    auto bounds = getLocalBounds();
    topBar.performLayout(bounds.removeFromTop(buttonHeight));


    codeEditor.setBounds (
        margin,
        2 * margin + buttonHeight,
        getWidth() - 2 * margin,
        getHeight() - 3 * margin - buttonHeight);
}

juce::String EditorComponent::getSource()
{
    return sourceCode.getAllContent();
}

void EditorComponent::setSource (const juce::String& source)
{
    sourceCode.replaceAllContent (source);
}
void EditorComponent::setStatus (const juce::String& status, juce::NotificationType notice)
{
    statusLabel.setText (status, notice);
}
