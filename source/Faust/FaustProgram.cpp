/*
Copyright (C) 2020, 2021 by Grégoire Locqueville <gregoireloc@gmail.com>
Copyright (C) 2022 by Kamil Kisiel <kamil@kamilkisiel.net>
Copyright (C) 2022 by GRAME <research@grame.fr>

This file is part of Amati.

Amati is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Amati is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Amati.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FaustProgram.h"
#include <faust/dsp/dsp.h>
#include <faust/dsp/interpreter-dsp.h>
#include <faust/dsp/llvm-dsp.h>
#include <memory>

std::list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;

static FaustProgram::ItemType apiToItemType (APIUI::ItemType type)
{
    using ItemType = FaustProgram::ItemType;
    switch (type)
    {
        case APIUI::kButton:
            return ItemType::Button;
        case APIUI::kCheckButton:
            return ItemType::CheckButton;
        case APIUI::kVSlider:
        case APIUI::kHSlider:
        case APIUI::kNumEntry:
            return ItemType::Slider;
        //TODO: implement BarGraphs
        case APIUI::kHBargraph:
        case APIUI::kVBargraph:
        default:
            return ItemType::Unavailable;
    }
}

static std::map<juce::String, juce::String> cstringMapToJuceString (const std::map<const char*, const char*>& cMap)
{
    std::map<juce::String, juce::String> juceStringMap;
    for (const auto& [key, value] : cMap)
    {
        juceStringMap[juce::String (key)] = juce::String (value);
    }
    return juceStringMap;
}

FaustProgram::FaustProgram (const juce::String& source, const Backend b, const int sampRate) : backend (b), sampleRate (sampRate)
{
    compileSource (source);
}

FaustProgram::~FaustProgram()
{
    // Delete in order.
    faustInterface.reset();
    if (midiIsOn)
    {
        midiInterface.reset (nullptr);
    }

    dspInstance.reset (nullptr);
    polyDspInstance.reset (nullptr);
    switch (backend)
    {
        case Backend::LLVM:
            deleteDSPFactory (static_cast<llvm_dsp_factory*> (dspFactory));
            break;
        case Backend::Interpreter:
            deleteInterpreterDSPFactory (static_cast<interpreter_dsp_factory*> (dspFactory));
        case Backend::PolyInterpreter:
        case Backend::PolyLLVM:
            break;
    }
}

int FaustProgram::matchPolyAndExtractVoices (const juce::String& input)
{
    // Define the regular expression pattern with a capture group for the number
    std::string stdStringInput = input.toStdString();
    std::regex pattern (R"(declare options "\[midi:on\]\[nvoices:(\d+)\]";)");
    std::smatch matches;

    // Check if the input string matches the pattern
    if (std::regex_search(stdStringInput, matches, pattern))
    {
        // Extract the number from the first capture group
        return std::stoi (matches[1].str());
    }
    return -1;
}

/**
 * Fills the parameters vector passed in
 */

void FaustProgram::fillParameters (std::vector<Parameter>& parameterVector, std::unique_ptr<APIUI>& interface)
{
    for (int i { 0 }; i < interface->getParamsCount(); i++)
    {
        parameterVector.push_back (
            {
                i,
                juce::String (interface->getParamLabel (i)),
                apiToItemType (interface->getParamItemType (i)),
                { interface->getParamMin (i), faustInterface->getParamMax (i) },
                interface->getParamInit (i),
                interface->getParamStep (i),
                cstringMapToJuceString (interface->getMetadata (i)),
                [&] (int index, double value) -> double { return interface->value2ratio (index, value); },
                [&] (int index, double ratio) -> double { return interface->ratio2value (index, ratio); },
            });
    }
}

void FaustProgram::initDspFactory (FaustProgram::Backend& back, const juce::String& source)
{
    const char* argv[] = { "" }; // compilation arguments
    std::string errorString;

    switch (back)
    {
        case Backend::LLVM:
            dspFactory = createDSPFactoryFromString (
                "faust", // program name
                source.toStdString(),
                0, // number of arguments
                argv,
                "", // compilation target; left empty to say we want to compile for this machine
                errorString);
            break;
        case Backend::Interpreter:
            dspFactory = createInterpreterDSPFactoryFromString (
                "faust", // program name
                source.toStdString(),
                0, // number of arguments
                argv,
                errorString);
            break;
        case Backend::PolyLLVM:
            polyDspFactory = std::make_unique<dsp_poly_factory>(createPolyDSPFactoryFromString (
                "faust",
                source.toStdString(),
                0,
                argv,
                "",
                errorString));
            break;
        case Backend::PolyInterpreter:
            polyDspFactory = std::make_unique<dsp_poly_factory>(createInterpreterPolyDSPFactoryFromString (
                "faust",
                source.toStdString(),
                0,
                argv,
                errorString));
            break;
        default:
        {
            juce::String message ("Invalid backend: ");
            message += static_cast<int> (backend);
            throw CompileError (message);
        }
    }

    if (!dspFactory)
    {
        throw CompileError (errorString);
    }
}

void FaustProgram::compileSource (const juce::String& source)
{
    midiIsOn = source.contains ("declare options \"[midi:on]\";");
    int polyVoices = matchPolyAndExtractVoices (source);
    if (polyVoices > 0)
    {
        backend = (backend == Backend::LLVM) ? Backend::PolyLLVM : Backend::PolyInterpreter;
        midiIsOn = true;
        poly = true;
    }
    initDspFactory (backend, source);
    faustInterface = std::make_unique<APIUI>();
    // initialize either the poly dsp instance or the regular dsp instance
    if (polyVoices > 0)
    {
        polyDspInstance.reset (polyDspFactory->createPolyDSPInstance (polyVoices, true, true));
        polyDspInstance->init (sampleRate);
        polyDspInstance->buildUserInterface (faustInterface.get());
        midi_handler = std::make_unique<juce_midi>();
        midiInterface = std::make_unique<MidiUI> (midi_handler.get());
        polyDspInstance->buildUserInterface (midiInterface.get());
        midi_handler->startMidi();
    }
    else
    {
        dspInstance.reset (dspFactory->createDSPInstance());
        dspInstance->init (sampleRate);
        dspInstance->buildUserInterface (faustInterface.get());
        if (midiIsOn)
        {
            buildMidi(dspInstance);
        }
    }

    fillParameters (parameters, faustInterface);
}

int FaustProgram::getParamCount() const
{
    return faustInterface->getParamsCount();
}

int FaustProgram::getNumInChannels() const
{
    return dspInstance->getNumInputs();
}

int FaustProgram::getNumOutChannels() const
{
    return dspInstance->getNumOutputs();
}

FaustProgram::Parameter FaustProgram::getParameter (const int index)
{
    if (index < 0)
    {
        jassertfalse;
    }
    return parameters[static_cast<unsigned long> (index)];
}

float FaustProgram::getValue (const int index) const
{
    if (index > 0 || index <= getParamCount())
        return static_cast<float> (faustInterface->getParamRatio (index));
    else
        return 0.0;
}

void FaustProgram::setValue (const int index, const float value) const
{
    if (index >= 0 && index <= getParamCount())
    {
        faustInterface->setParamRatio (index, value);
    }
}

void FaustProgram::compute (const int samples, const float* const* in, float* const* out) const
{
    if (!poly)
    {
        dspInstance->compute (samples, const_cast<float**> (in), const_cast<float**> (out));
    }
    else
    {
        polyDspInstance->compute (samples, const_cast<float**> (in), const_cast<float**> (out));
    }
}

void FaustProgram::handleMidiBuffer (juce::MidiBuffer& message) const
{
    if (midiIsOn)
    {
        midi_handler->decodeBuffer (message);
    }
}

void FaustProgram::setSampleRate (const int sr)
{
    if (sr > 0)
    {
        sampleRate = sr;
    }
    else
    {
        jassertfalse;
    }
}

void FaustProgram::buildMidi (std::unique_ptr<dsp>& dsp)
{
    midi_handler = std::make_unique<juce_midi>();
    midiInterface = std::make_unique<MidiUI> (midi_handler.get());
    dsp->buildUserInterface (midiInterface.get());
    midi_handler->startMidi();
}
