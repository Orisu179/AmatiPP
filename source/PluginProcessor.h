#pragma once
#include "Faust/FaustProgram.h"
#include <clap-juce-extensions/clap-juce-extensions.h>
#include <juce_audio_processors/juce_audio_processors.h>

inline juce::String paramIdForIdx (const int idx)
{
    return juce::String ("Param") + juce::String (idx);
}
inline juce::String paramIdForIdx (const size_t idx)
{
    return paramIdForIdx (static_cast<int> (idx));
}

class PluginProcessor final : public juce::AudioProcessor, juce::ValueTree::Listener, public clap_juce_extensions::clap_juce_audio_processor_capabilities, private juce::Timer
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    void handleMidi (const juce::MidiMessage&);

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // For compiling faust program ---------
    bool compileSource (const juce::String&);
    juce::String getSourceCode();
    void setBackend (FaustProgram::Backend);
    void setPlayingState (bool);
    void timerCallback() override;

    struct FaustParameter
    {
        juce::String id;
        FaustProgram::Parameter programParameter;
    };
    std::vector<FaustParameter> getFaustParameter() const;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void valueTreePropertyChanged (
        juce::ValueTree& tree,
        const juce::Identifier& property) override;
    juce::String sourceCode = "";
    FaustProgram::Backend backend {};
    std::unique_ptr<FaustProgram> faustProgram {};
    bool playing { false };
    bool readyToPlay { false };
    juce::AudioProcessorValueTreeState valueTreeState;
    juce::MidiBuffer midiBuffer;

    // used to copy the input buffers
    juce::AudioBuffer<float> tmpBufferIn;
    juce::AudioBuffer<float> tmpBufferOut;
    double sampRate {};

    void updateDspParameters() const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
