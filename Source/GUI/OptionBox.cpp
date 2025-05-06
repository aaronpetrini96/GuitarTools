/*
  ==============================================================================

    OptionBox.cpp
    Created: 21 Apr 2025 12:06:19am
    Author:  Aaron Petrini

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OptionBox.h"
#include "LookAndFeel.h"

OptionBox::OptionBox(const std::vector<juce::String>& options, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID)
: attachment(apvts, parameterID.getParamID(), comboBox)
{
    for (int i{1}; i < (options.size() + 1) ; ++i)
        comboBox.addItem(options.at(i-1), i);

    comboBox.setSelectedId(1);
//    comboBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.1f));
    comboBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    comboBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    
    comboBox.setBounds(0, 0, 80, 20);
    addAndMakeVisible(comboBox);
    
    setSize(80, 20);
    
    setLookAndFeel(ComboBoxLookAndFeel::get());
    
}

OptionBox::~OptionBox()
{
}



void OptionBox::resized()
{

}

