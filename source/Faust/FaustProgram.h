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

#pragma once
#include "juce_core/juce_core.h"
#include "FaustMidi.h"
#include <faust/dsp/dsp.h>
#include <faust/gui/APIUI.h>
#include <faust/gui/MidiUI.h>
#include <faust/gui/GUI.h>

class FaustProgram final : public GUI {
public:
    class CompileError final : public std::runtime_error
    {
    public:
        explicit CompileError (const char* message) : std::runtime_error (message) {}
        explicit CompileError (const std::string& message) : std::runtime_error (message) {}
        explicit CompileError (const juce::String& message) : CompileError (message.toStdString()) {}
    };

    enum class ItemType {
        // Unavailabe is for when we don't/can't include the UI element;
        Unavailable,
        Slider,
        Button,
        CheckButton,
    };

    enum class Backend {
        LLVM,
        Interpreter,
    };

    /// Construct a Faust Program.
    /// @throws CompileError
    FaustProgram (const juce::String& source, Backend, int sampRate);
    ~FaustProgram() override;

    [[nodiscard]] int getParamCount() const;
    [[nodiscard]] int getNumInChannels() const;
    [[nodiscard]] int getNumOutChannels() const;

    struct Parameter
    {
        juce::String label;
        ItemType type;
        juce::Range<double> range;
        double init;
        double step;
    };
    Parameter getParameter(int idx);
    void convertNormaliseRange(int index, float value) const;

    [[nodiscard]] float getValue(int index) const;
    void setValue(int idx, float) const;
    void setSampleRate(int);

    void compute (int sampleCount, const float* const* input, float* const* output) const;

    void handleMidi(juce::MidiBuffer&) const;

private:
    void compileSource (const juce::String&);

    static bool midiOn(const juce::String&);

    Backend backend;

    dsp_factory* dspFactory{};
    std::unique_ptr<dsp> dspInstance;
    std::unique_ptr<APIUI> faustInterface;
    std::unique_ptr<FaustMidi> midi_handler;
    std::vector<Parameter> parameters;

    int sampleRate;
};