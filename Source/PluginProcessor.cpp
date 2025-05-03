/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarToolsAudioProcessor::GuitarToolsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GuitarToolsAudioProcessor::~GuitarToolsAudioProcessor()
{
}

//==============================================================================



//==============================================================================
const juce::String GuitarToolsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarToolsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarToolsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarToolsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarToolsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarToolsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarToolsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarToolsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarToolsAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarToolsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarToolsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
//    spec.numChannels = getTotalNumInputChannels();
    spec.numChannels = 1;

    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    

    updateFilters();


}

void GuitarToolsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarToolsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GuitarToolsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
//
    auto chainSettings = getChainSettings(treeState);

    
    updateFilters();
    
    juce::dsp::AudioBlock<float> block {buffer};
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(0);
    if(totalNumInputChannels > 1)
        auto rightBlock = block.getSingleChannelBlock(1);
        

    juce::dsp::ProcessContextReplacing<float> leftContext {leftBlock};
    juce::dsp::ProcessContextReplacing<float> rightContext {rightBlock};
    
    if(chainSettings.pluginBypass == 0)
    {
        leftChain.process(leftContext);
        rightChain.process(rightContext);
    }
    
}

//==============================================================================
bool GuitarToolsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarToolsAudioProcessor::createEditor()
{
    return new GuitarToolsAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarToolsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{

//    juce::MemoryOutputStream mos(destData, true);
//    treeState.state.writeToStream(mos);
    
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GuitarToolsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{

//    auto valueTree = juce::ValueTree::readFromData(data, sizeInBytes);
//    if (valueTree.isValid())
//    {
//        treeState.replaceState(valueTree);
//        updateFilters();
//    }
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(treeState.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        treeState.replaceState(state);
        updateFilters();
    }
}

void GuitarToolsAudioProcessor::savePreset(const juce::File& file)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml) { xml->writeTo(file);}
}

void GuitarToolsAudioProcessor::loadPreset(const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse(file));
    if (xml && xml->hasTagName(treeState.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        treeState.replaceState(state);
        updateFilters();
    }
}

//void GuitarToolsAudioProcessor::loadDefaultPreset()
//{
//    auto* xmlData = juce::BinaryData::DefaultPreset_xml;
//    auto xmlSize = juce::BinaryData::DefaultPreset_xmlSize;
//    
//    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (juce::String::fromUTF8 (xmlData, xmlSize)));
//    
//    if (xml && xml->hasTagName (treeState.state.getType()))
//    {
//        juce::ValueTree state = juce::ValueTree::fromXml (*xml);
//        treeState.replaceState (state);
//        updateFilters();
//    }
//}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GuitarToolsAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

//    ========== HICUT LOW CUT ===========
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("LowCut Freq",1),"LowCut Freq",juce::NormalisableRange<float>(20.f, 4000.f, 1.f, 0.5f),40.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("HighCut Freq",1),"HighCut Freq",juce::NormalisableRange<float>(4000.f, 20000.f, 1.f, 1.f),14000.f));
    
    
    juce::StringArray stringArray;
    for (int i {0}; i < 4; ++i) {
        juce::String str;
        str << (12 + 12*i);
        str << " dB/Oct";
        stringArray.add(str);
    }
    
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("LowCut Slope", 1),"LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("HighCut Slope", 1),"HighCut Slope", stringArray, 0));

    
    
//    ========== HIGH/LOW SHELVE  ===========

    juce::StringArray presenceFreqArray {"4.5 kHz", "6 kHz", "8 kHz"};
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Presence Freq", 1), "Presence Freq", presenceFreqArray, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Presence Gain", 1), "Presence Gain", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f),3.f));
    

    juce::StringArray depthFreqArray {"60 Hz", "100 Hz", "170 Hz"};
//    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Depth Freq", 1), "Depth Freq",
//               juce::NormalisableRange<float>(60.f, 500.f, 1.f, 1.f), 80.f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Depth Freq", 1), "Depth Freq", depthFreqArray, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Depth Gain", 1), "Depth Gain", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f),0.f));
//    ========== RESONANCE TAMER ===========
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Reso Freq",1),"Reso Freq",juce::NormalisableRange<float>(2000.f, 6000.f, 1.f, 1.f),3850.f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Reso Bypass", 1), "Reso Bypass", false));
    
//    ========== MUD KILLER TAMER ===========
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Mud Freq", 1),"Mud Freq",juce::NormalisableRange<float>(40.f, 500.f, 1.f, 1.f),120.f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Mud Bypass", 1), "Mud Bypass", false));
    
//    ========== BYPASS ===========
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Plugin Bypass", 1), "Bypass", false));
//    ========== RETURN LAYOUT ===========
    return layout;
}

//==============================================================================
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& treeState)
{
    ChainSettings settings;
    
    settings.lowCutFreq = treeState.getRawParameterValue("LowCut Freq")->load();
    settings.lowCutSlope = static_cast<Slope>(treeState.getRawParameterValue("LowCut Slope")->load());
    settings.highCutFreq = treeState.getRawParameterValue("HighCut Freq")->load();
    settings.highCutSlope = static_cast<Slope>(treeState.getRawParameterValue("HighCut Slope")->load());
    settings.mudFreq = treeState.getRawParameterValue("Mud Freq")->load();
    settings.mudBypass = treeState.getRawParameterValue("Mud Bypass")->load() < 0.5f;
    settings.presenceIndex = static_cast<int>(treeState.getRawParameterValue("Presence Freq")->load());
    settings.presenceGain = treeState.getRawParameterValue("Presence Gain")->load();
//    settings.depthFreq = treeState.getRawParameterValue("Depth Freq")->load();
    settings.depthIndex = static_cast<int>(treeState.getRawParameterValue("Depth Freq")->load());
    settings.depthGain = treeState.getRawParameterValue("Depth Gain")->load();
    settings.resoBypass = treeState.getRawParameterValue("Reso Bypass")->load() < 0.5f;
    settings.resoFreq = treeState.getRawParameterValue("Reso Freq")->load();
//    presence
    if (settings.presenceIndex == 0)
    {
        settings.presenceFreq = 4500.f;
    }
    else if (settings.presenceIndex == 1)
    {
        settings.presenceFreq = 6000.f;
    }
    else if (settings.presenceIndex == 2)
    {
        settings.presenceFreq = 8000.f;
    }
//    Depth
    if (settings.depthIndex == 0)
    {
        settings.depthFreq = 60.f;
    }
    else if (settings.depthIndex == 1)
    {
        settings.depthFreq = 100.f;
    }
    else if (settings.depthIndex == 2)
    {
        settings.depthFreq = 170.f;
    }

    settings.pluginBypass = treeState.getRawParameterValue("Plugin Bypass")->load() > 0.5;
    
    return settings;
}

//==============================================================================
void GuitarToolsAudioProcessor::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}
//==============================================================================
void GuitarToolsAudioProcessor::updateBellFitlers(const ChainSettings& chainSettings)
{

    auto resoCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.resoFreq, 10.f, juce::Decibels::decibelsToGain(-6.f));

    leftChain.setBypassed<ChainPositions::Reso>(chainSettings.resoBypass);
    rightChain.setBypassed<ChainPositions::Reso>(chainSettings.resoBypass);

    updateCoefficients(leftChain.get<ChainPositions::Reso>().coefficients, resoCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Reso>().coefficients, resoCoefficients);
    
    auto mudCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.mudFreq, 2.5f, juce::Decibels::decibelsToGain(-2.f));
    
    leftChain.setBypassed<ChainPositions::Mud>(chainSettings.mudBypass);
    rightChain.setBypassed<ChainPositions::Mud>(chainSettings.mudBypass);
    
    updateCoefficients(leftChain.get<ChainPositions::Mud>().coefficients, mudCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Mud>().coefficients, mudCoefficients);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateShelfFilters(const ChainSettings& chainSettings)
{

    auto presCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), chainSettings.presenceFreq, 0.707f, juce::Decibels::decibelsToGain(chainSettings.presenceGain));
    
    updateCoefficients(leftChain.get<ChainPositions::Presence>().coefficients, presCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Presence>().coefficients, presCoefficients);
    
    auto depthCoefficientes = juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), chainSettings.depthFreq, 0.707f, juce::Decibels::decibelsToGain(chainSettings.depthGain));
    updateCoefficients(leftChain.get<ChainPositions::Depth>().coefficients, depthCoefficientes);
    updateCoefficients(rightChain.get<ChainPositions::Depth>().coefficients, depthCoefficientes);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateLowCutFilters(const ChainSettings& chainSettings)
{
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, getSampleRate(), (chainSettings.lowCutSlope + 1) * 2);
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();
    updateCutFilter(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
}
//==============================================================================
void GuitarToolsAudioProcessor::updateHighCutFilters(const ChainSettings& chainSettings)
{
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq, getSampleRate(), (chainSettings.highCutSlope + 1) * 2);
    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();
    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}


//==============================================================================
void GuitarToolsAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(treeState);
    updateBellFitlers(chainSettings);
    updateShelfFilters(chainSettings);
    updateLowCutFilters(chainSettings);
    updateHighCutFilters(chainSettings);
}



//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================



// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarToolsAudioProcessor();
}




