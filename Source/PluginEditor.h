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
#include "GUI/LevelMeter.h"
#include "Utils/PresetManager.h"
//==============================================================================
/**
*/
class GuitarToolsAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
//private juce::Timer
{
public:
    GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor&);
    ~GuitarToolsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
private:

    GuitarToolsAudioProcessor& audioProcessor;
    
    juce::GroupComponent cutFiltersGroup, buttonsGroup, shelfFiltersGroup;
    MainLookAndFeel mainLF;
    
    RotaryKnob lowCutFreqKnob {"Low Cut", audioProcessor.treeState, juce::ParameterID("LowCut Freq", 1)," Hz"};
    RotaryKnob highCutFreqKnob {"High Cut", audioProcessor.treeState, juce::ParameterID("HighCut Freq", 1)," Hz"};
    
    RotaryKnob lowShelfGainKnob {"Depth", audioProcessor.treeState, juce::ParameterID("Depth Gain", 1), " dB", true};
    RotaryKnob highShelfGainKnob {"Presence", audioProcessor.treeState, juce::ParameterID("Presence Gain", 1), " dB", true};
    
    
    juce::TextButton presence1 {"1"};
    juce::TextButton presence2 {"2"};
    juce::TextButton presence3 {"3"};
    std::vector<juce::TextButton*> presenceButtons = {&presence1, &presence2, &presence3};
    float choiceCount {0}, normalizedValue{0};
    void setPresenceFreq(const int&);
    void updatePresenceButtons(const int&);
    void setShelfFilterButtonStyle(juce::TextButton&);
    
    juce::TextButton depth1 {"1"};
    juce::TextButton depth2 {"2"};
    juce::TextButton depth3 {"3"};
    std::vector<juce::TextButton*> depthButtons = {&depth1, &depth2, &depth3};
    void setDepthFreq(const int&);
    void updateDepthButtons(const int&);

    std::vector<juce::String> slopeOptions {"12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct"};
    OptionBox lowCutSlopeBox {slopeOptions, audioProcessor.treeState, juce::ParameterID("LowCut Slope", 1)};
    OptionBox highCutSlopeBox {slopeOptions, audioProcessor.treeState, juce::ParameterID("HighCut Slope", 1)};
    
    std::vector<juce::String> oversamplingOptions { "Off (OS)", "2x", "4x", "8x" };
    OptionBox oversamplingBox {oversamplingOptions, audioProcessor.treeState, juce::ParameterID("Oversampling",1)};
    
    std::vector<juce::String> ratioChoices = {"1:1","1.4:1","2:1","3:1","4:1","8:1","10:1","20:1","50:1","100:1"};
    OptionBox compRatioBox {ratioChoices, audioProcessor.treeState, juce::ParameterID("Ratio",1)};
    
    Button resoButton {"Tame Resonance", audioProcessor.treeState, juce::ParameterID("Reso Bypass", 1)};
    Button mudButton {"Kill Mud", audioProcessor.treeState, juce::ParameterID("Mud Bypass", 1)};
    Button compBypassButton {"Control Low-End", audioProcessor.treeState, juce::ParameterID("Comp Bypass",1)};
    
    HorizontalSlider resoFreqSlider {"Freqy", audioProcessor.treeState, juce::ParameterID("Reso Freq", 1), " Hz"};
    HorizontalSlider mudFreqSlider {"Freq", audioProcessor.treeState, juce::ParameterID("Mud Freq", 1), " Hz"};
    HorizontalSlider compThresholdSlider {"Thresh", audioProcessor.treeState, juce::ParameterID("Threshold", 1), " dB"};
    
    ImgButton bypassButton{"Bypass Button", audioProcessor.treeState, juce::ParameterID("Plugin Bypass",1)};
    
    PresetManager presetManager;
    juce::ComboBox presetBox;
    juce::TextButton savePresetButton {"Save"};

    LevelMeter inputMeter;
    LevelMeter outputMeter;
    
    
    
    
//    HorizontalSlider inputGain {"InGain", audioProcessor.treeState, juce::ParameterID("Comp Gain In",1), " dB"};
//    HorizontalSlider outputGain {"OutGain", audioProcessor.treeState, juce::ParameterID("Comp Gain Out",1), " dB"};
    
    
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessorEditor)
};
