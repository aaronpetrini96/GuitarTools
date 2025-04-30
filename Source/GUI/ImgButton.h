/*
  ==============================================================================

    ImgButton.h
    Created: 30 Apr 2025 3:18:17am
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
//==============================================================================
/*
*/
class ImgButton  : public juce::Component
{
public:
    ImgButton(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID);
    ~ImgButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::ImageButton button;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImgButton)
};
