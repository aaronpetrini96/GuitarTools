/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48,
};

struct ChainSettings
{
    float lowCutFreq {0}, highCutFreq{0};
    float resoFreq {0}, mudFreq {0}, presenceFreq {0}, depthFreq{0};
    float presenceGain {0}, depthGain{0};
    int presenceIndex{0}, depthIndex{0};
    
    bool resoBypass {false}, mudBypass {false}, pluginBypass {false};


    Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& treeState);

//==============================================================================
/**
*/
class GuitarToolsAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GuitarToolsAudioProcessor();
    ~GuitarToolsAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);
    void loadDefaultPreset();
    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState treeState {*this, nullptr, "Parameters", createParameterLayout()};

private:
    

    
//    Declare Filter types and Process Chains
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, Filter, Filter, Filter, CutFilter>;
    MonoChain leftChain, rightChain;
    
    enum ChainPositions
    {
        LowCut,
        Reso,
        Mud,
        Presence,
        Depth,
        HighCut
    };
    
    void updateBellFitlers(const ChainSettings& chainSettings);
    void updateShelfFilters(const ChainSettings& chainSettings);
    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);
    void updateFilters();
    
    using Coefficients = Filter::CoefficientsPtr; //alias for type needed to update coef
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);
    
//    =====  UPDATE CUT FILTERS =====
    
    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& cutCoefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, cutCoefficients[Index]);
        chain.template setBypassed<Index>(false);
    }
    
    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain, const CoefficientType& cutCoefficients, const Slope& slope)
    {

        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);
        
        
        switch (slope)
        {
            case Slope_48:
            {
                update<3>(chain, cutCoefficients);
            }
                
            case Slope_36:
            {
                update<2>(chain, cutCoefficients);
            }

            case Slope_24:
            {
                update<1>(chain, cutCoefficients);
            }

            case Slope_12:
            {
                update<0>(chain, cutCoefficients);
            }
            
        }
        
    }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarToolsAudioProcessor)
};
