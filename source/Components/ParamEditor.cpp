
/* Copyright (C) 2020 by Grégoire Locqueville <gregoireloc@gmail.com> Copyright (C) 2022 by Kamil Kisiel <kamil@kamilkisiel.net>
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

#include "ParamEditor.h"

// https://forum.juce.com/t/how-to-get-parameterchanged-to-be-called-for-push-button/29052/17?u=kisielk

class MomentaryButton : public juce::TextButton
{
public:
    MomentaryButton (const juce::String& label) : juce::TextButton (label)
    {
    }

    void mouseDown (const juce::MouseEvent&) override
    {
        setState (Button::buttonDown);

        if (!getToggleState())
            setToggleState (true, juce::sendNotification);
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        setState (Button::buttonOver);

        if (getToggleState())
            setToggleState (false, juce::sendNotification);
    }
};

ParamEditor::ParamEditor (juce::AudioProcessorValueTreeState& vts) : valueTreeState (vts), builder (vts)
{
}

ParamEditor::~ParamEditor() noexcept
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    labels.clear();
    components.clear();
}

void ParamEditor::updateParameters (const std::vector<PluginProcessor::FaustParameter>& params)
{
    sliderAttachments.clear();
    buttonAttachments.clear();
    labels.clear();
    components.clear();

    using Type = FaustProgram::ItemType;
    for (const auto& param : params)
    {
        juce::Component* component = nullptr;
        const auto& p = param.programParameter;
        switch (p.type)
        {
            case Type::Slider:
            {
                builder.setParameterData (p);
                auto slider = builder.getSlider();
                AmatiSliderAttachment* attachment = builder.getAttachment (slider, param.id);
                auto* label = new juce::Label();
                label->attachToComponent (slider, false);
                label->setText (p.label, juce::dontSendNotification);

                component = slider;
                labels.add (label);
                sliderAttachments.add (attachment);

                addAndMakeVisible (slider);
                addAndMakeVisible (label);
                builder.reset();
                break;
            }
            case Type::Button:
            {
                auto* button = new MomentaryButton (p.label);
                button->setClickingTogglesState (true);
                auto* attachment = new juce::AudioProcessorValueTreeState::ButtonAttachment (
                    valueTreeState, param.id, *button);

                component = button;
                buttonAttachments.add (attachment);

                addAndMakeVisible (button);
                break;
            }
            case Type::CheckButton:
            {
                auto* button = new juce::TextButton (p.label);
                button->setClickingTogglesState (true);
                auto* attachment = new juce::AudioProcessorValueTreeState::ButtonAttachment (
                    valueTreeState, param.id, *button);

                component = button;
                buttonAttachments.add (attachment);

                addAndMakeVisible (button);
                break;
            }
            case Type::Unavailable:
                continue;
        }
        components.add (component);
        if(component) {
            component->getProperties().set ("type", static_cast<int> (p.type));
        }
    }
    resized();
}

void ParamEditor::resized()
{
    constexpr int margin = 50;
    constexpr int sliderHeight = 30;
    constexpr int sideMargin = 10;

    using Type = FaustProgram::ItemType;
    for (int i = 0; i < components.size(); ++i)
    {
        auto* comp = components[i];
        const auto& props = comp->getProperties();
        auto type = static_cast<Type> (static_cast<int> (props.getWithDefault ("type", 0)));
        auto width = getWidth() - sideMargin * 2;
        if (type == Type::Button || type == Type::CheckButton)
        {
            width = 100;
        }
        comp->setBounds (
            sideMargin,
            (1 + i) * margin + i * sliderHeight,
            width,
            sliderHeight);
    }
}

