#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarToolsAudioProcessorEditor::GuitarToolsAudioProcessorEditor (GuitarToolsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    


    presence1.onClick = [this]() {setPresenceFreq(0);};
    presence2.onClick = [this]() {setPresenceFreq(1);};
    presence3.onClick = [this]() {setPresenceFreq(2);};
    
    int index = static_cast<int>(audioProcessor.treeState.getRawParameterValue("Presence Freq")->load());
    updatePresenceButtons(index);
    
    
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
        setPresenceButtonStyle(*presenceButtons[i]);
        shelfFiltersGroup.addAndMakeVisible(*presenceButtons[i]);
    }
    shelfFiltersGroup.addAndMakeVisible(lowShelfGainKnob);
    addAndMakeVisible(shelfFiltersGroup);
    
    
    addAndMakeVisible(bypassButton);
    setLookAndFeel(&mainLF);
    

    setSize (500, 350);
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
    g.drawFittedText("GUITAR TOOLS v1", (getLocalBounds().getWidth() * 0.31), getLocalBounds().getHeight() * 0.932, 200, 20, juce::Justification::centred, 1);
}

void GuitarToolsAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    int y = bounds.getHeight() * 0.015;
    int height = bounds.getHeight() * 0.915;
    auto leftMargin = bounds.getWidth() * 0.02;
    auto presenceButtonsSize = leftMargin * 2.5;
    auto groupWidth = bounds.getWidth() * 0.245;
    
    bypassButton.setBounds(bounds.getWidth() * 0.935, bounds.getHeight() * 0.932, bypassButton.getWidth(), bypassButton.getHeight());
    
    cutFiltersGroup.setBounds(leftMargin, y, groupWidth, height);
    shelfFiltersGroup.setBounds(bounds.getWidth() * 0.74, y, groupWidth, height);
    buttonsGroup.setBounds(cutFiltersGroup.getRight() + leftMargin, y, shelfFiltersGroup.getX() - cutFiltersGroup.getRight() - leftMargin * 2, height);
    
//    CUT FILTER GROUP
    lowCutFreqKnob.setTopLeftPosition((cutFiltersGroup.getWidth()-lowCutFreqKnob.getWidth()) * 0.5, leftMargin * 1.5);
    highCutFreqKnob.setBounds(lowCutFreqKnob.getX(), lowCutFreqKnob.getHeight() * 1.5, lowCutFreqKnob.getWidth(), lowCutFreqKnob.getHeight());
    
    lowCutSlopeBox.setTopLeftPosition((cutFiltersGroup.getWidth()-lowCutSlopeBox.getWidth()) * 0.5, lowCutFreqKnob.getHeight() * 1.2);
    highCutSlopeBox.setTopLeftPosition((cutFiltersGroup.getWidth()-highCutSlopeBox.getWidth()) * 0.5, lowCutFreqKnob.getHeight() * 2.6);
    
    
//    BUTTONS GROUP
    resoButton.setTopLeftPosition((buttonsGroup.getWidth() - resoButton.getWidth()) * 0.5, leftMargin * 3);
    resoFreqSlider.setBounds(resoButton.getX() * 0.6, resoButton.getHeight() * 1.4, resoFreqSlider.getWidth(), resoFreqSlider.getHeight());
    
    mudButton.setTopLeftPosition((buttonsGroup.getWidth() - resoButton.getWidth()) * 0.5, resoButton.getBottom() * 1.5);
    mudFreqSlider.setBounds(resoFreqSlider.getX(), mudButton.getHeight() * 3.15, mudButton.getWidth() * 1.5, mudButton.getHeight());

    
//    SHELF GROUP
    highShelfGainKnob.setTopLeftPosition((shelfFiltersGroup.getWidth() - highShelfGainKnob.getWidth()) * 0.5, leftMargin * 1.5);
    presence1.setBounds(highShelfGainKnob.getX() * 0.52, highShelfGainKnob.getHeight() * 1.2, presenceButtonsSize, presenceButtonsSize);
    presence2.setBounds((presence1.getX() + presence1.getWidth()) + leftMargin, highShelfGainKnob.getHeight() * 1.2, presenceButtonsSize, presenceButtonsSize);
    presence3.setBounds((presence2.getX() + presence1.getWidth()) + leftMargin, highShelfGainKnob.getHeight() * 1.2, presenceButtonsSize, presenceButtonsSize);
    
    lowShelfGainKnob.setBounds(highShelfGainKnob.getX(), highCutFreqKnob.getY(), highShelfGainKnob.getWidth(), highShelfGainKnob.getHeight());
    
}


void GuitarToolsAudioProcessorEditor::setPresenceButtonStyle(juce::TextButton& button)
{
    button.setClickingTogglesState(true);
//    button.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
//    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green.withAlpha(0.75f));
    
    button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(100, 100, 110).darker(0.5f));
    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(230, 165, 70));
    button.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    
    
    button.setSize(25, 25);
//    button.setLookAndFeel(ButtonLookAndFeel::get());
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


void GuitarToolsAudioProcessorEditor::updatePresenceButtons(const int& selectedIndex)
{
    presence1.setToggleState(selectedIndex == 0, juce::dontSendNotification);
    presence2.setToggleState(selectedIndex == 1, juce::dontSendNotification);
    presence3.setToggleState(selectedIndex == 2, juce::dontSendNotification);
    
}
