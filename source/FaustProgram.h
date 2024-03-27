//
// Created by tyler on 3/24/24.
// Wrapper around Faust stuff to be used
//

#ifndef FAUSTVST_FAUSTPROGRAM_H
#define FAUSTVST_FAUSTPROGRAM_H

#endif //FAUSTVST_FAUSTPROGRAM_H

#include <juce_core/juce_core.h>
#include <faust/dsp/dsp.h>
#include <faust/gui/APIUI.h>

class FaustProgram
{
    enum Backend {
        LLVM,
        Interpreter
    };

    FaustProgram(juce::String source, Backend backend, int samples);
    ~FaustProgram();
};