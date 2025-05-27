#include "CompressorBand.h"


void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void CompressorBand::updateCompressorSettings()
{
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float>& buffer)
{
    auto preRMS = computeRMSLevel(buffer);
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    auto isItBypased = bypassed->get();
    context.isBypassed = !isItBypased;
    
    compressor.process(context);
    
    auto postRMS = computeRMSLevel(buffer);
    
    auto convertTodB = [](auto input)
    {
        return juce::Decibels::gainToDecibels(input);
    };
    
    rmsInputLeveldB.store(convertTodB(preRMS));
    rmsOutputLeveldB.store(convertTodB(postRMS));
}
