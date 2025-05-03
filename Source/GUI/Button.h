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
class Button  : public juce::Component, private juce::Timer
{
public:
    Button(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID);
    ~Button() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    

private:
    juce::ToggleButton button;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;
    juce::Label label;
    
    const juce::Colour backgroundColourWhenOff = juce::Colour(100, 100, 110).darker(1.2f);
    const juce::Colour backgroundColourWhenOn = juce::Colour(0xFF40C4FF).darker(1.5f);
    juce::Colour currentBackground {backgroundColourWhenOff};
    
    // Animation state
    float animationProgress = 0.0f;
    bool animatingForward = true;
    
    void startAnimation();
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Button)
};


