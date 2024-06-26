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

#include <faust/dsp/interpreter-dsp.h>
#include <faust/dsp/llvm-dsp.h>

#include <memory>

FaustProgram::FaustProgram (const juce::String& source, Backend b, int sampRate) : backend (b), sampleRate (sampRate)
{
    compileSource (source);
}

FaustProgram::~FaustProgram()
{
    // Delete in order.
    faustInterface.reset (nullptr);
    dspInstance.reset (nullptr);

    switch (backend)
    {
        case FaustProgram::Backend::LLVM:
            deleteDSPFactory (static_cast<llvm_dsp_factory*> (dspFactory));
            break;
        case FaustProgram::Backend::Interpreter:
            deleteInterpreterDSPFactory (static_cast<interpreter_dsp_factory*> (dspFactory));
            break;
    }
}

void FaustProgram::compileSource (const juce::String& source)
{
    const char* argv[] = { "" }; // compilation arguments
    std::string errorString;

    switch (backend)
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
        default:
        {
            juce::String message ("Invalid backend: ");
            message += int (backend);
            throw CompileError (message);
        }
    }

    if (!dspFactory)
    {
        throw CompileError (errorString);
    }

    dspInstance.reset (dspFactory->createDSPInstance());
    dspInstance->init (sampleRate);
    faustInterface = std::make_unique<APIUI>();
    dspInstance->buildUserInterface (faustInterface.get());
}

int FaustProgram::getParamCount()
{
    return faustInterface->getParamsCount();
}

int FaustProgram::getNumInChannels()
{
    return dspInstance->getNumInputs();
}

int FaustProgram::getNumOutChannels()
{
    return dspInstance->getNumOutputs();
}

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
        case APIUI::kHBargraph:
        case APIUI::kVBargraph:
        default:
            return ItemType::Unavailable;
    }
}

FaustProgram::Parameter FaustProgram::getParameter (int idx)
{
    return { juce::String (faustInterface->getParamLabel (idx)),
        apiToItemType (faustInterface->getParamItemType (idx)),
        { faustInterface->getParamMin (idx), faustInterface->getParamMax (idx) },
        faustInterface->getParamInit (idx),
        faustInterface->getParamStep (idx) };
}

float FaustProgram::getValue (int index)
{
    if (index > 0 || index <= getParamCount())
        return static_cast<float> (faustInterface->getParamRatio (index));
    else
        return 0.0;
}

void FaustProgram::setValue (int index, float value)
{
    if (index > 0 || index <= getParamCount())
    {
        faustInterface->setParamRatio (index, value);
    }
}

void FaustProgram::compute (int samples, const float* const* in, float* const* out)
{
    dspInstance->compute (samples, const_cast<float**> (in), const_cast<float**> (out));
}
void FaustProgram::setSampleRate (int sr)
{
    if(sr > 0){
        sampleRate = sr;
    } else {
        jassertfalse;
    }
}
