/*
  ==============================================================================

    Button.h
    Created: 21 Apr 2025 10:26:58pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"
//==============================================================================
/*
*/
class Button  : public juce::Component
{
public:
    Button(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID);
    ~Button() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool getToggleState();
   
    
    juce::ToggleButton button;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;
    juce::Label label;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Button)
};


