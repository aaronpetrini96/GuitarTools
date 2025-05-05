#pragma once
#include <JuceHeader.h>
//#include "../Utilities.h"
#define MIN_FREQ 20.f
#define MAX_FREQ 20000.f
#define NEGATIVE_INFINITY -72.f
#define MAX_DECIBELS 12.f
#define MIN_THRESHOLD -60.f

struct CompressorBand
{
    juce::AudioParameterFloat* attack {nullptr};
    juce::AudioParameterFloat* release {nullptr};
    juce::AudioParameterFloat* threshold {nullptr};
    juce::AudioParameterChoice* ratio {nullptr};
    juce::AudioParameterBool* bypassed {nullptr};
//    juce::AudioParameterBool* mute {nullptr};
//    juce::AudioParameterBool* solo {nullptr};
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    
    void updateCompressorSettings();
    
    void process(juce::AudioBuffer<float>& buffer);
    
    float getRMSInputLeveldB() const {return rmsInputLeveldB;}
    float getRMSOutputLeveldB() const {return rmsOutputLeveldB;}
    
private:
    juce::dsp::Compressor<float> compressor;
    
    std::atomic<float> rmsInputLeveldB {NEGATIVE_INFINITY};
    std::atomic<float> rmsOutputLeveldB {NEGATIVE_INFINITY};
    
    template <typename T>
    float computeRMSLevel (const T& buffer)
    {
        int numChannels = static_cast<int>(buffer.getNumChannels());
        int numSamples = static_cast<int>(buffer.getNumSamples());
        auto rms = 0.f;
        for (int chan = 0; chan< numChannels; ++chan)
            rms += buffer.getRMSLevel(chan,0,numSamples);
        rms/= static_cast<float>(numChannels);
        return rms;
    }
    
};
