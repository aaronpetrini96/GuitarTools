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
    button.setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::white);
//    button.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::white);
//    button.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colour(160, 105, 134));
//    button.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::white.withAlpha(0.5f));

    
    addAndMakeVisible(button);
    button.setBounds(0, 0, 120, 50);
    
    setSize(130, 60);
    setLookAndFeel(ButtonLookAndFeel::get());
    
    // Hook up state change to start animation
    button.onClick = [this]()
    {
        startAnimation();
    };

}

Button::~Button()
{
}

void Button::paint (juce::Graphics& g)
{
    
    auto bounds = button.getLocalBounds().toFloat();
    auto cornersize = bounds.getHeight() * 0.25f;
    auto buttonRect = bounds.reduced(1.f, 1.f).withTrimmedBottom(1.f);


//    currentBackground = button.getToggleState() ? backgroundColourWhenOn : backgroundColourWhenOff;


    //background
    g.setColour(currentBackground);
    g.fillRoundedRectangle(buttonRect, cornersize);

    // Outline
//    g.setColour(juce::Colour(138, 138, 138));
//    g.drawRoundedRectangle(buttonRect, cornersize, 2.f);
 
}

void Button::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void Button::startAnimation()
{
    animationProgress = 0.0f;
    startTimerHz(60); // 60 FPS
    animatingForward = button.getToggleState();
}

void Button::timerCallback()
{
    animationProgress += 0.2f; // Speed (tweak this if needed)

    if (animationProgress >= 1.0f)
    {
        animationProgress = 1.0f;
        stopTimer();
    }

    float t = animationProgress;
    if (!animatingForward)
        t = 1.0f - t;

    currentBackground = backgroundColourWhenOff.interpolatedWith(backgroundColourWhenOn, t);

    button.repaint();
}
