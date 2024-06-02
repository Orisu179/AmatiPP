#include "PluginProcessor.h"

#include <memory>
#include "PluginEditor.h"

//==============================================================================
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (int i = 0; i < 1; i++)
    {
        auto id = juce::ParameterID (paramIdForIdx (i), 1);
        auto name = juce::String ("Parameter ") + juce::String (i);
        layout.add (std::make_unique<juce::AudioParameterFloat> (id, name, 0.f, 1.f, 0.f));
    }
    return layout;
}
namespace Id {
    const juce::Identifier sourceCode("source_code");
    const juce::Identifier settings("settings");
    const juce::Identifier backend("backend");
}

PluginProcessor::PluginProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
                                     AudioProcessor (BusesProperties()
                                                         .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
                                     valueTreeState (*this, nullptr, "parameters", createParameterLayout())
{
    valueTreeState.state.addListener (this);
}

PluginProcessor::~PluginProcessor() = default;

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    sampleRate = sampRate;

    // numChannelsIn and numChannelsOut should be equal
    // (and probably equal to 0),
    // but we get both just in case

    int numChannelsIn  = tmpBufferIn.getNumChannels ();
    int numChannelsOut = tmpBufferOut.getNumChannels ();

    tmpBufferIn  = juce::AudioBuffer<float> (numChannelsIn,  samplesPerBlock);
    tmpBufferOut = juce::AudioBuffer<float> (numChannelsOut, samplesPerBlock);
    compileSource(sourceCode);
    playing = true;
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    playing = false;
    faustProgram.reset(nullptr);
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
    #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
#endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    int numSamples = buffer.getNumSamples();
    // The host should not give us more samples than expected
    // If it does, we resize our internal buffers
    if(numSamples > tmpBufferIn.getNumChannels()) {
        tmpBufferIn.setSize(tmpBufferIn.getNumChannels(), numSamples);
        tmpBufferOut.setSize(tmpBufferOut.getNumChannels(), numSamples);
    }

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if(!faustProgram){
        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        // This is here to avoid people getting screaming feedback
        // when they first compile a plugin, but obviously you don't need to keep
        // this code if your algorithm always overwrites all the output channels.
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());
    } else {
        updateDspParameters();

        // here, the buffered are copied into tmpBufferIn, then processed into tmpBufferOut
        // tmpBufferOut would then be copied into buffer again
        // This is to make sure that computation is done in a controlled environment
        // i.e. buffers with known number of channels
        // TODO: are there any other ways to improve this? Or does the copy have to be done everytime?
        for (int chan = 0; (chan < totalNumInputChannels) && (chan < tmpBufferIn.getNumChannels()); ++chan)
            tmpBufferIn.copyFrom (chan, 0, buffer, chan, 0, numSamples);

        // clear remaining channels
        for (int chan = totalNumInputChannels; chan < tmpBufferIn.getNumChannels(); ++chan)
            tmpBufferIn.clear(chan, 0, numSamples);

        faustProgram->compute(numSamples, tmpBufferIn.getArrayOfReadPointers(), tmpBufferOut.getArrayOfWritePointers());
        for(int chan = 0; (chan < totalNumOutputChannels) && (chan < tmpBufferOut.getNumChannels()); ++chan)
            buffer.copyFrom(chan, 0, tmpBufferOut, chan, 0, numSamples);

        for(int chan = tmpBufferOut.getNumChannels(); chan < totalNumOutputChannels; ++chan)
            buffer.clear(chan, 0, numSamples);
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this, valueTreeState);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::Logger::getCurrentLogger()->writeToLog("Saving preset");

    //create parent element
    juce::XmlElement preset = juce::XmlElement("amati_preset");

    //Add source code
    juce::XmlElement* sourceTag = preset.createNewChildElement("source");
    sourceTag->addTextElement(sourceCode);
    auto state = valueTreeState.copyState().createXml();
    preset.addChildElement(new juce::XmlElement(*state.get()));
    // DBG(preset.toString());
    // stores tree on desk
    copyXmlToBinary(preset, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto* logger = juce::Logger::getCurrentLogger();
    logger->writeToLog("Loading preset");

    std::unique_ptr<juce::XmlElement> preset(getXmlFromBinary(data, sizeInBytes));

    if(!preset.get()) {
        logger->writeToLog("Invalid preset: invalid XML");
        return;
    }
    DBG(preset->toString());

    if(!preset->hasTagName("amati_preset")){
        logger->writeToLog("Invalid preset: wrong root tag");
        return;
    }
    auto* parameters = preset->getChildByName(valueTreeState.state.getType());
    if (!parameters) {
        logger->writeToLog("Invalid preset: missing parameters");
        return;
    }

    auto* source = preset->getChildByName("source");
    if (!source) {
        logger->writeToLog("Invalid preset: missing source");
        return;
    }

    valueTreeState.replaceState(juce::ValueTree::fromXml(*parameters));
    sourceCode = source->getAllSubText();
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

bool PluginProcessor::compileSource (juce::String source) {
    if(!playing) {
        return false;
    }
    switch(backend) {
        case FaustProgram::Backend::LLVM:
            juce::Logger::writeToLog("Compiling with LLVM backend...");
            break;
        case FaustProgram::Backend::Interpreter:
            juce::Logger::writeToLog("Compiling with Interpreter backend...");
            break;
    }
    try {
        faustProgram = std::make_unique<FaustProgram>(source, backend, static_cast<int>(sampRate));
        juce::Logger::getCurrentLogger()->writeToLog("Compilation Complete! Using new program.");
    } catch(FaustProgram::CompileError& e) {
        auto* logger = juce::Logger::getCurrentLogger();
        logger->writeToLog("Compilation Failed");
        logger->writeToLog(e.what());
        return false;
    }
    sourceCode = source;
    // Update internal buffers
    int inChans  = faustProgram->getNumInChannels  ();
    int outChans = faustProgram->getNumOutChannels ();

    tmpBufferIn.setSize  (inChans,  tmpBufferIn.getNumSamples  ());
    tmpBufferOut.setSize (outChans, tmpBufferOut.getNumSamples ());
    return true;
}


void PluginProcessor::updateDspParameters() {
    int paramCount = faustProgram->getParamCount();
    for (int i = 0; i < paramCount; ++i) {
       juce::String id = paramIdForIdx(i);
       float value = *valueTreeState.getRawParameterValue(id);
       faustProgram->setValue(i, value);
    }
}
void PluginProcessor::setBackend (FaustProgram::Backend newBackend) {
    DBG("setBackend: " << int(newBackend));
    backend = newBackend;
    compileSource(sourceCode);
}

std::vector<PluginProcessor::FaustParameter> PluginProcessor::getFaustParameter() const {
    std::vector<PluginProcessor::FaustParameter> params;
    if(!faustProgram) {
        return params;
    }
    for(int i = 0; i<faustProgram->getParamCount(); i++)
        params.push_back({ paramIdForIdx(i), faustProgram->getParameter(i)});
    return params;
}

juce::String PluginProcessor::getSourceCode() {
    return sourceCode;
}

void PluginProcessor::valueTreePropertyChanged (juce::ValueTree& tree, const juce::Identifier& property) {
    if(property == Id::backend) {
        int newBackend = tree[property];
        setBackend(static_cast<FaustProgram::Backend>(newBackend - 1));
    }
    DBG("property change: " << tree.getType() << " " << property);
}

