/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/RotaryKnob.h"
#include "GUI/LookAndFeel.h"
#include "GUI/OptionBox.h"
#include "GUI/Button.h"
#include "GUI/HorizontalSlider.h"
#include "GUI/ImgButton.h"

//==============================================================================
/**
*/
class GuitarToolsAudioProcessorEditor  : public juce::AudioProcessorEditor
//private juce::Timer
{
public:
    GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor&);
    ~GuitarToolsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setPresenceFreq(const int&);
    void updatePresenceButtons(const int&);
    void setPresenceButtonStyle(juce::TextButton&);
    
    
private:

    GuitarToolsAudioProcessor& audioProcessor;
    
    juce::GroupComponent cutFiltersGroup, buttonsGroup, shelfFiltersGroup;
    MainLookAndFeel mainLF;
    
    RotaryKnob lowCutFreqKnob {"Low Cut", audioProcessor.treeState, juce::ParameterID("LowCut Freq", 1)," Hz"};
    RotaryKnob highCutFreqKnob {"High Cut", audioProcessor.treeState, juce::ParameterID("HighCut Freq", 1)," Hz"};
    
    RotaryKnob lowShelfGainKnob {"Depth Gain", audioProcessor.treeState, juce::ParameterID("Depth Gain", 1), " dB", true};
    RotaryKnob highShelfGainKnob {"Presence Gain", audioProcessor.treeState, juce::ParameterID("Presence Gain", 1), " dB", true};
    
    juce::TextButton presence1 {"1"};
    juce::TextButton presence2 {"2"};
    juce::TextButton presence3 {"3"};
    std::vector<juce::TextButton*> presenceButtons = {&presence1, &presence2, &presence3};
    float choiceCount {0}, normalizedValue{0};
//    const juce::Colour backgroundColourWhenOff = juce::Colour(100, 100, 110).darker(1.2f);
//    const juce::Colour backgroundColourWhenOn = juce::Colour(0xFF40C4FF).darker(1.5f);
//    juce::Colour currentBackground {backgroundColourWhenOff};
    
//    // Animation state
//    float animationProgress = 0.0f;
//    bool animatingForward = false;
//    float textScale = 1.0f; // Default text scale
//    void startAnimation(juce::TextButton&);
//    void timerCallback() override;

    std::vector<juce::String> slopeOptions {"12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct"};
    OptionBox lowCutSlopeBox {slopeOptions, audioProcessor.treeState, juce::ParameterID("LowCut Slope", 1)};
    OptionBox highCutSlopeBox {slopeOptions, audioProcessor.treeState, juce::ParameterID("HighCut Slope", 1)};
    
    Button resoButton {"Tame Resonance", audioProcessor.treeState, juce::ParameterID("Reso Bypass", 1)};
    Button mudButton {"Kill Mud", audioProcessor.treeState, juce::ParameterID("Mud Bypass", 1)};
    
    HorizontalSlider resoFreqSlider {"Frequency", audioProcessor.treeState, juce::ParameterID("Reso Freq", 1), " Hz"};
    HorizontalSlider mudFreqSlider {"Frequency", audioProcessor.treeState, juce::ParameterID("Mud Freq", 1), " Hz"};
    
    ImgButton bypassButton{"Bypass Button", audioProcessor.treeState, juce::ParameterID("Plugin Bypass",1)};
    

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
