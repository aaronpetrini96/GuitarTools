#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), presetManager(p, presetBox)
{
    
    startTimerHz(30);
//  PRESENCE BUTTONS
    presence1.onClick = [this]() {setShelfFilterFreq("Presence Freq", 0);};
    presence2.onClick = [this]() {setShelfFilterFreq("Presence Freq", 1);};
    presence3.onClick = [this]() {setShelfFilterFreq("Presence Freq", 2);};
    int indexPresence = static_cast<int>(audioProcessor.treeState.getRawParameterValue("Presence Freq")->load());
    updatePresenceButtons(indexPresence);
    
//  DEPTH BUTTONS
    depth1.onClick = [this]() {setShelfFilterFreq("Depth Freq", 0);};
    depth2.onClick = [this]() {setShelfFilterFreq("Depth Freq", 1);};
    depth3.onClick = [this]() {setShelfFilterFreq("Depth Freq", 2);};
    int indexDepth = static_cast<int>(audioProcessor.treeState.getRawParameterValue("Depth Freq")->load());
    updateDepthButtons(indexDepth);
    
//  PRESET BUTTONS
    setShelfFilterButtonStyle(savePresetButton);
    savePresetButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(100, 100, 110).darker(0.5f));
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this] {presetManager.savePreset();};
    
//    IN-OUT GAIN
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);

    
//  PRESET DROPDOWN MENU
    presetBox.setTextWhenNothingSelected("Select Preset");
    presetBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    presetBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    presetBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    presetBox.onChange = [this]() {presetManager.presetSelected();};
    presetManager.refreshPresetList();
    addAndMakeVisible(presetBox);
    
//    OVERSAMPLING
//    oversamplingBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
//    oversamplingBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
//    oversamplingBox.setLookAndFeel(ComboBoxLookAndFeel::get());
//    oversamplingBox.setSize(60, 25);
    addAndMakeVisible(oversamplingBox);
    
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
    resoFreqSlider.setVisible(resoButton.getToggleState());
    resoButton.onClick = [this](){resoFreqSlider.setVisible(resoButton.getToggleState());};
    
    buttonsGroup.addAndMakeVisible(mudButton);
    buttonsGroup.addAndMakeVisible(mudFreqSlider);
    mudFreqSlider.setVisible(mudButton.getToggleState());
    mudButton.onClick = [this](){mudFreqSlider.setVisible(mudButton.getToggleState());};
    
    buttonsGroup.addAndMakeVisible(compBypassButton);
    buttonsGroup.addAndMakeVisible(compThresholdSlider);
    
    compThresholdSlider.setVisible(compBypassButton.getToggleState());
    compRatioBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    compRatioBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    compRatioBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    buttonsGroup.addAndMakeVisible(compRatioBox);
    compRatioBox.setVisible(compBypassButton.getToggleState());
    compBypassButton.onClick = [this]()
    {
        bool state = compBypassButton.getToggleState();
        compThresholdSlider.setVisible(state);
        compRatioBox.setVisible(state);
    };
    addAndMakeVisible(buttonsGroup);
    
    
    shelfFiltersGroup.setText("Expression");
    shelfFiltersGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    
    shelfFiltersGroup.addAndMakeVisible(highShelfGainKnob);
    shelfFiltersGroup.addAndMakeVisible(lowShelfGainKnob);
    for (int i {0}; i < shelfFiltersButtons.size(); ++i)
    {
        setShelfFilterButtonStyle(*shelfFiltersButtons[i]);
        shelfFiltersGroup.addAndMakeVisible(*shelfFiltersButtons[i]);
    }
    
    addAndMakeVisible(shelfFiltersGroup);
    
    addAndMakeVisible(bypassButton);
//    addAndMakeVisible(inputGain);
    setLookAndFeel(&mainLF);

    
//    addAndMakeVisible(inputMeter);
//    addAndMakeVisible(outputMeter);
    
    
    setSize (500, 400);
    
}

GuitarToolsAudioProcessorEditor::~GuitarToolsAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}
//==============================================================================



//==============================================================================
void GuitarToolsAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromRGB(23, 24, 21).darker(1.5f), getHeight(), juce::Colour::fromRGB(23, 24, 21), getHeight() * 0.4));
    g.fillRect(getLocalBounds());
  
    g.setColour (juce::Colours::white.withAlpha(0.9f));
    g.setFont(16.f);
    g.getCurrentFont();
    g.drawFittedText("GUITAR TOOLS v2", -25, getLocalBounds().getHeight() * 0.83, 200, 20, juce::Justification::centred, 1);
    
    clipLight(g);
}

void GuitarToolsAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    int y = bounds.getHeight() * 0.015;
    int height = bounds.getHeight() * 0.8;
    auto leftMargin = bounds.getWidth() * 0.02;
    auto presenceButtonsSize = leftMargin * 2.5;
    auto groupWidth = bounds.getWidth() * 0.245;
    auto lastLineY = bounds.getHeight() * 0.83;
    
//    BYPASS
    bypassButton.setBounds(bounds.getWidth() * 0.93, lastLineY, bypassButton.getWidth(), bypassButton.getHeight());
//   PRESET BOX
    presetBox.setBounds(bounds.getWidth() * 0.31, lastLineY, resoFreqSlider.getWidth() * 0.8, highCutSlopeBox.getHeight());
    savePresetButton.setBounds(presetBox.getRight() * 1.05, lastLineY, presetBox.getWidth() * 0.5, presetBox.getHeight());
//    OVERSAMPLING
    oversamplingBox.setBounds(bounds.getWidth() * 0.75, lastLineY, oversamplingBox.getWidth(), bypassButton.getHeight());
    
//    IN OUT GAIN
    inputGainSlider.setBounds(8.0, lastLineY * 1.05, inputGainSlider.getWidth(), inputGainSlider.getHeight());
    outputGainSlider.setBounds(bounds.getWidth() * 0.585, lastLineY * 1.05, outputGainSlider.getWidth(), outputGainSlider.getHeight());
    

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
    auto middleButtonsGroup = (buttonsGroup.getWidth() - resoButton.getWidth()) * 0.53;
    resoButton.setTopLeftPosition(middleButtonsGroup, leftMargin * 2.1);
    resoFreqSlider.setBounds(resoButton.getX() * 0.6, resoButton.getBottom() * 0.89, 150, 30);
    
    mudButton.setTopLeftPosition(middleButtonsGroup, resoButton.getBottom() * 1.35);
    mudFreqSlider.setBounds(resoFreqSlider.getX(), mudButton.getBottom() * 0.95, mudButton.getWidth() * 1.5, mudButton.getHeight());
    
    compBypassButton.setTopLeftPosition(middleButtonsGroup, mudButton.getBottom() * 1.18);
    compThresholdSlider.setBounds(resoFreqSlider.getX(),compBypassButton.getBottom() * 0.965 , compThresholdSlider.getWidth(), compThresholdSlider.getHeight());
    compRatioBox.setBounds(compBypassButton.getX() * 1.5, highCutSlopeBox.getY(), lowCutSlopeBox.getWidth(), lowCutSlopeBox.getHeight());

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


void GuitarToolsAudioProcessorEditor::setShelfFilterFreq(const juce::String& parameterName, const int& index)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.treeState.getParameter(parameterName)))
    {
        param->beginChangeGesture();
        choiceCount = static_cast<float>(param->choices.size() - 1);
        normalizedValue = static_cast<float>(index) / choiceCount;
        param->setValueNotifyingHost(normalizedValue);
        param->endChangeGesture();
        if(parameterName == "Presence Freq")
            updatePresenceButtons(index);
        if(parameterName == "Depth Freq")
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



void GuitarToolsAudioProcessorEditor::timerCallback()
{
   
// === Clipping light logic ===
    
    bool inClipping = audioProcessor.clipFlagIn.exchange(false);
    bool outClipping = audioProcessor.clipFlagOut.exchange(false);
    if (inClipping)
        inputGainSlider.setClippingStatus(true);
    if (outClipping)
        outputGainSlider.setClippingStatus(true);
    
    if (inClipping == true || outClipping == true)
    {
        isClippingLightOn = true;
        clipLightHoldCounter = 15; // light stays on for 15 ticks
        clipPopScale = 1.3f;       // pop to 1.3x size when clipping
    }
    
    if (clipLightHoldCounter > 0)
    {
        --clipLightHoldCounter;
        
        // Gradually shrink pop effect back to normal
        clipPopScale -= 0.02f;
        if (clipPopScale < 1.0f)
            clipPopScale = 1.0f;
    }
    else
    {
        inputGainSlider.setClippingStatus(false);
        outputGainSlider.setClippingStatus(false);
        isClippingLightOn = false;
        clipPopScale = 1.0f;
    }
    
    repaint(); // triggers paint() to draw updated light
    
}

void GuitarToolsAudioProcessorEditor::clipLight(juce::Graphics& g)
{
    juce::Colour activeColor = juce::Colours::red.withAlpha(0.9f);
    juce::Colour offColor = juce::Colours::green.withAlpha(0.75f);
    juce::Colour currentColor = isClippingLightOn ? activeColor : offColor;
   
    g.setColour(currentColor);
    g.setFont(juce::Font(16.0f * clipPopScale, juce::Font::bold));
    g.drawFittedText("CLIP", getWidth() * 0.3, getHeight() * 0.92, 200, 20, juce::Justification::centred, 1);
}
