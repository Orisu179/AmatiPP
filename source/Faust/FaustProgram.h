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
#include "FaustMidi.h"
#include "juce_core/juce_core.h"
#include <faust/dsp/dsp.h>
#include <faust/gui/APIUI.h>
#include <faust/gui/GUI.h>
#include <faust/gui/MidiUI.h>
#include <faust/midi/juce-midi.h>

class FaustProgram final : public GUI
{
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

    enum class MetaData {
        tooltip,
        hidden,
        unit,
        scale,
        style,
        acc,
        gyr,
        screencolor,
        midi
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
    [[nodiscard]] std::vector<int> getMidiIndex() const;
    [[nodiscard]] int getNumInChannels() const;
    [[nodiscard]] int getNumOutChannels() const;

    struct Parameter
    {
        int index;
        juce::String label;
        ItemType type;
        juce::Range<double> range;
        double init;
        double step;
        std::map<juce::String, juce::String> metaData;
        std::function<double (int, double)> value2Ratio;
        std::function<double (int, double)> ratio2Value;
    };
    Parameter getParameter (int);

    [[nodiscard]] float getValue (int) const;
    void setValue (int, float) const;
    void setSampleRate (int);

    void compute (int sampleCount, const float* const* input, float* const* output) const;
    void handleMidiBuffer (juce::MidiBuffer&) const;

private:
    void compileSource (const juce::String&);
    void populateMidiParameters();

    Backend backend;

    dsp_factory* dspFactory {};
    std::unique_ptr<dsp> dspInstance;
    std::shared_ptr<APIUI> faustInterface;
    std::unique_ptr<juce_midi> midi_handler;
    std::unique_ptr<MidiUI> midiInterface;
    std::vector<Parameter> parameters;
    std::vector<int> midiId;

    int sampleRate;
    bool midiIsOn { false };
};
