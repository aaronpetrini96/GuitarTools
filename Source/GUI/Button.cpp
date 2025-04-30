/*
  ==============================================================================

    Button.cpp
    Created: 21 Apr 2025 10:26:58pm
    Author:  Aaron Petrini

  ==============================================================================p
*/

#include <JuceHeader.h>
#include "Button.h"


//==============================================================================
Button::Button(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID)
: attachment(apvts, parameterID.getParamID(), button)
{
    
    button.setButtonText(text);
    button.setToggleable(true);
    button.setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::black);
    button.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
    
    button.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colour(160, 105, 134));

    addAndMakeVisible(button);
    button.setBounds(0, 0, 120, 50);
    
    setSize(130, 60);
    setLookAndFeel(ButtonLookAndFeel::get());

}

Button::~Button()
{
}

void Button::paint (juce::Graphics& g)
{
    
    auto bounds = button.getLocalBounds().toFloat();
    auto cornersize = bounds.getHeight() * 0.25f;
    auto buttonRect = bounds.reduced(1.f, 1.f).withTrimmedBottom(1.f);
 
    
    g.setColour(juce::Colours::black.withAlpha(0.1f));
    g.fillRoundedRectangle(buttonRect, cornersize);
    

//    g.setColour(Colors::Button::outline);
    g.setColour(juce::Colour(160, 105, 134));
    g.drawRoundedRectangle(buttonRect, cornersize, 2.f);

}

void Button::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


bool Button::getToggleState()
{
    return button.getToggleState();
}
