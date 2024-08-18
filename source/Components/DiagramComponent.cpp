#include "DiagramComponent.h"

DiagramComponent::DiagramComponent() : path("/home/tyler/.config"), showSvg (true)
{
    juce::File svgCloud = juce::File("/home/tyler/temp/simple-cloud.svg");
    if(svgCloud.exists() && showSvg)
    {
        svg = juce::Drawable::createFromSVGFile(svgCloud);
        addAndMakeVisible(svg.get());
    }
}

DiagramComponent::~DiagramComponent()
{

}

void DiagramComponent::resized()
{
//    if(showSvg)
//    {
//        svg->setBounds(getLocalBounds());
//    }
}

void DiagramComponent::paint (juce::Graphics& g)
{
//    svg->draw(g, 1.0f);
}

void DiagramComponent::setSource (juce::String source)
{
    std::string standardString = path.toStdString();
    std::basic_string<char> errorMsg;
    int argc = 0;
    const char* argv2[64];
    argv2[argc++] = "-svg";
    argv2[argc++] = "-0";
    argv2[argc++] = standardString.c_str();
    argv2[argc] = nullptr;
    if(!generateAuxFilesFromString("AmatiSVG", "process = vslider(\"Volume\", 0.5, 0, 1, 0.025);", argc, argv2, errorMsg))
    {
        DBG(errorMsg);
    } else {
        juce::File svgFile = juce::File(path + "/AmatiSVG/process.svg");
        svg = juce::Drawable::createFromSVGFile(svgFile);
    }
}

void DiagramComponent::setSvg (bool value)
{
    showSvg = value;
}
