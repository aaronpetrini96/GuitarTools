#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

class PresetManager
{
public:
    PresetManager(GuitarToolsAudioProcessor& processor, juce::ComboBox& presetBox);
    
    void savePreset();
    void loadPreset();
    void presetSelected();
    void refreshPresetList();
    
    
private:
    juce::File getPresetFolder();
    GuitarToolsAudioProcessor& audioProcessor;
    juce::ComboBox& presetBox;
    std::unique_ptr<juce::FileChooser> fileChooser;
};
