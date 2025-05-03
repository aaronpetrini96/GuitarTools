#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
//  PRESENCE BUTTONS
    presence1.onClick = [this]() {setPresenceFreq(0);};
    presence2.onClick = [this]() {setPresenceFreq(1);};
    presence3.onClick = [this]() {setPresenceFreq(2);};
    int indexPresence = static_cast<int>(audioProcessor.treeState.getRawParameterValue("Presence Freq")->load());
    updatePresenceButtons(indexPresence);
    
//  DEPTH BUTTONS
    depth1.onClick = [this]() {setDepthFreq(0);};
    depth2.onClick = [this]() {setDepthFreq(1);};
    depth3.onClick = [this]() {setDepthFreq(2);};
    int indexDepth = static_cast<int>(audioProcessor.treeState.getRawParameterValue("Depth Freq")->load());
    updateDepthButtons(indexDepth);
    
//  PRESET BUTTONS
    setShelfFilterButtonStyle(savePresetButton);
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this] {savePreset();};
    
//  PRESET DROPDOWN MENU
    presetBox.setTextWhenNothingSelected("Select Preset");
    presetBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    presetBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    presetBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    presetBox.onChange = [this]() {presetSelected();};
    refreshPresetList();
    addAndMakeVisible(presetBox);
    
//    GROUPS
    cutFiltersGroup.setText("Cut Filters");
    cutFiltersGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    
    cutFiltersGroup.addAndMakeVisible(lowCutFreqKnob);
    cutFiltersGroup.addAndMakeVisible(highCutFreqKnob);
    cutFiltersGroup.addAndMakeVisible(lowCutSlopeBox);
    cutFiltersGroup.addAndMakeVisible(highCutSlopeBox);
    addAndMakeVisible(cutFiltersGroup);
    
    
    buttonsGroup.setText("Magic Buttons");
    buttonsGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    
    buttonsGroup.addAndMakeVisible(resoButton);
    buttonsGroup.addAndMakeVisible(resoFreqSlider);
    buttonsGroup.addAndMakeVisible(mudButton);
    buttonsGroup.addAndMakeVisible(mudFreqSlider);
    addAndMakeVisible(buttonsGroup);
    
    shelfFiltersGroup.setText("Expression");
    shelfFiltersGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    
    shelfFiltersGroup.addAndMakeVisible(highShelfGainKnob);
    for (int i {0}; i < presenceButtons.size(); ++i)
    {
        setShelfFilterButtonStyle(*presenceButtons[i]);
        shelfFiltersGroup.addAndMakeVisible(*presenceButtons[i]);
    }
    shelfFiltersGroup.addAndMakeVisible(lowShelfGainKnob);
    for (int i {0}; i < depthButtons.size(); ++i)
    {
        setShelfFilterButtonStyle(*depthButtons[i]);
        shelfFiltersGroup.addAndMakeVisible(*depthButtons[i]);
    }
    addAndMakeVisible(shelfFiltersGroup);
    
    addAndMakeVisible(bypassButton);
    setLookAndFeel(&mainLF);

    setSize (500, 360);

}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}
//==============================================================================



//==============================================================================
void GuitarToolsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (juce::Colours::whitesmoke);
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromRGB(23, 24, 21).darker(1.5f), getHeight(), juce::Colour::fromRGB(23, 24, 21), getHeight() * 0.4));
    g.fillRect(getLocalBounds());
  
    g.setColour (juce::Colours::white.withAlpha(0.9f));
    g.setFont(16.f);
    g.getCurrentFont();
    g.drawFittedText("GUITAR TOOLS v1", -25, getLocalBounds().getHeight() * 0.932, 200, 20, juce::Justification::centred, 1);
}

void GuitarToolsAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    int y = bounds.getHeight() * 0.015;
    int height = bounds.getHeight() * 0.9;
    auto leftMargin = bounds.getWidth() * 0.02;
    auto presenceButtonsSize = leftMargin * 2.5;
    auto groupWidth = bounds.getWidth() * 0.245;
    
//    BYPASS
    bypassButton.setBounds(bounds.getWidth() * 0.93, bounds.getHeight() * 0.932, bypassButton.getWidth(), bypassButton.getHeight());
    
//    GROUPS
    cutFiltersGroup.setBounds(leftMargin, y, groupWidth, height);
    shelfFiltersGroup.setBounds(bounds.getWidth() * 0.74, y, groupWidth, height);
    buttonsGroup.setBounds(cutFiltersGroup.getRight() + leftMargin, y, shelfFiltersGroup.getX() - cutFiltersGroup.getRight() - leftMargin * 2, height);
    
//    CUT FILTER GROUP
    lowCutFreqKnob.setTopLeftPosition((cutFiltersGroup.getWidth()-lowCutFreqKnob.getWidth()) * 0.5, leftMargin * 1.5);
    highCutFreqKnob.setBounds(lowCutFreqKnob.getX(), lowCutFreqKnob.getHeight() * 1.5, lowCutFreqKnob.getWidth(), lowCutFreqKnob.getHeight());
    lowCutSlopeBox.setTopLeftPosition((cutFiltersGroup.getWidth()-lowCutSlopeBox.getWidth()) * 0.5, lowCutFreqKnob.getHeight() * 1.2);
    highCutSlopeBox.setTopLeftPosition((cutFiltersGroup.getWidth()-highCutSlopeBox.getWidth()) * 0.5, lowCutFreqKnob.getHeight() * 2.6);
    
//    BUTTONS GROUP
    auto middleButtonsGroup = (buttonsGroup.getWidth() - resoButton.getWidth()) * 0.5;
    resoButton.setTopLeftPosition(middleButtonsGroup, leftMargin * 3);
    resoFreqSlider.setBounds(resoButton.getX() * 0.6, resoButton.getHeight() * 1.4, resoFreqSlider.getWidth(), resoFreqSlider.getHeight());
    
    mudButton.setTopLeftPosition(middleButtonsGroup, resoButton.getBottom() * 1.5);
    mudFreqSlider.setBounds(resoFreqSlider.getX(), mudButton.getHeight() * 3.15, mudButton.getWidth() * 1.5, mudButton.getHeight());
    
    presetBox.setBounds(bounds.getWidth() * 0.31, bypassButton.getY(), resoFreqSlider.getWidth() * 0.8, highCutSlopeBox.getHeight());
    savePresetButton.setBounds(presetBox.getRight() * 1.05, bypassButton.getY(), presetBox.getWidth() * 0.5, presetBox.getHeight());

//    SHELF GROUP
    highShelfGainKnob.setTopLeftPosition((shelfFiltersGroup.getWidth() - highShelfGainKnob.getWidth()) * 0.5, leftMargin * 1.5);
    presence1.setBounds(highShelfGainKnob.getX() * 0.52, highShelfGainKnob.getHeight() * 1.2, presenceButtonsSize, presenceButtonsSize);
    presence2.setBounds((presence1.getX() + presence1.getWidth()) + leftMargin, highShelfGainKnob.getHeight() * 1.2, presenceButtonsSize, presenceButtonsSize);
    presence3.setBounds((presence2.getX() + presence1.getWidth()) + leftMargin, highShelfGainKnob.getHeight() * 1.2, presenceButtonsSize, presenceButtonsSize);
    
    lowShelfGainKnob.setBounds(highShelfGainKnob.getX(), highCutFreqKnob.getY(), highShelfGainKnob.getWidth(), highShelfGainKnob.getHeight());
    depth1.setBounds(lowShelfGainKnob.getX() * 0.52, lowShelfGainKnob.getHeight() * 2.59, presenceButtonsSize, presenceButtonsSize);
    depth2.setBounds((depth1.getX() + depth1.getWidth()) + leftMargin, lowShelfGainKnob.getHeight() * 2.59, presenceButtonsSize, presenceButtonsSize);
    depth3.setBounds((depth2.getX() + depth1.getWidth()) + leftMargin, lowShelfGainKnob.getHeight() * 2.59, presenceButtonsSize, presenceButtonsSize);
    
}

//==============================================================================
void GuitarToolsAudioProcessorEditor::setShelfFilterButtonStyle(juce::TextButton& button)
{
    button.setClickingTogglesState(true);
//    button.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(100, 100, 110).darker(0.5f));
//    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(230, 165, 70));
    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(0xFF40C4FF).darker(1.f));
    button.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    
    button.setSize(25, 25);

}

void GuitarToolsAudioProcessorEditor::setPresenceFreq(const int& index)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.treeState.getParameter("Presence Freq")))
    {
        param->beginChangeGesture();
        choiceCount = static_cast<float>(param->choices.size() - 1);
        normalizedValue = static_cast<float>(index) / choiceCount;
        param->setValueNotifyingHost(normalizedValue);
        param->endChangeGesture();
        updatePresenceButtons(index);
    }
}

void GuitarToolsAudioProcessorEditor::setDepthFreq(const int& index)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.treeState.getParameter("Depth Freq")))
    {
        param->beginChangeGesture();
        choiceCount = static_cast<float>(param->choices.size() - 1);
        normalizedValue = static_cast<float>(index) / choiceCount;
        param->setValueNotifyingHost(normalizedValue);
        param->endChangeGesture();
        updateDepthButtons(index);
    }
}


void GuitarToolsAudioProcessorEditor::updatePresenceButtons(const int& selectedIndex)
{
    presence1.setToggleState(selectedIndex == 0, juce::dontSendNotification);
    presence2.setToggleState(selectedIndex == 1, juce::dontSendNotification);
    presence3.setToggleState(selectedIndex == 2, juce::dontSendNotification);
}

void GuitarToolsAudioProcessorEditor::updateDepthButtons(const int& selectedIndex)
{
    depth1.setToggleState(selectedIndex == 0, juce::dontSendNotification);
    depth2.setToggleState(selectedIndex == 1, juce::dontSendNotification);
    depth3.setToggleState(selectedIndex == 2, juce::dontSendNotification);
}


//==============================================================================
//==============================================================================
void GuitarToolsAudioProcessorEditor::savePreset()
{
    auto presetFolder = getPresetFolder();
    if (!presetFolder.exists())
        presetFolder.createDirectory();
    
    fileChooser = std::make_unique<juce::FileChooser> ("Save Preset", presetFolder, "*.xml");
    
    auto fileChooserFlags = juce::FileBrowserComponent::saveMode;
    
    fileChooser->launchAsync (fileChooserFlags, [this] (const juce::FileChooser& fileChooser)
    {
        juce::File selectedFile = fileChooser.getResult();
        
        if (selectedFile.existsAsFile() || selectedFile.create())
        {
            // Save the preset to the selected file
            audioProcessor.savePreset(selectedFile);
            DBG("Preset saved to: " + selectedFile.getFullPathName());
            
            // Optionally, refresh the preset list in the UI or do other actions
            refreshPresetList();
        }
        else
        {
            DBG("Error: Failed to save preset. File creation failed.");
        };
        
    });

}

void GuitarToolsAudioProcessorEditor::loadPreset()
{
    juce::FileChooser chooser ("Load Preset", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory), "*.xml");

    auto file = chooser.getResult();
    audioProcessor.loadPreset (file);  // Calls your processor’s loadPreset
}

void GuitarToolsAudioProcessorEditor::presetSelected()
{
    auto selectedId = presetBox.getSelectedId();
    if (selectedId > 0)
    {
        auto presetName = presetBox.getItemText(selectedId - 1);
        auto presetFile = getPresetFolder().getChildFile(presetName + ".xml");
        audioProcessor.loadPreset(presetFile);
    }
}

void GuitarToolsAudioProcessorEditor::refreshPresetList()
{
    presetBox.clear();
    auto presetFolder = getPresetFolder();
    presetFolder.createDirectory();
    
    auto files = presetFolder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false);
    
    int index = 1;
    for (auto& file : files)
    {
        presetBox.addItem(file.getFileNameWithoutExtension(), index);
        ++index;
    }
}

juce::File GuitarToolsAudioProcessorEditor::getPresetFolder()
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("GuitarTools/Presets");
}
