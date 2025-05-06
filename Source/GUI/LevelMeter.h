#pragma once
#include <JuceHeader.h>

class LevelMeter : public juce::Component, private juce::Timer
{
public:
    LevelMeter() { startTimerHz(30); } // 30 FPS for smoothness

    void setLevels(float left, float right, int numChannels)
    {
        numInputChannels = numChannels;

        targetLevelL = left;
        targetLevelR = (numChannels > 1) ? right : left;

        peakHoldL = std::max(peakHoldL, targetLevelL);
        peakHoldR = std::max(peakHoldR, targetLevelR);
        peakHoldTime = 0;
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto meterWidth = bounds.getWidth() * 0.4f;

        auto drawBar = [&] (float x, float level, float peak)
        {
            // Background
            g.setColour(juce::Colours::darkgrey);
            g.fillRoundedRectangle(x, bounds.getY(), meterWidth, bounds.getHeight(), 3.0f);

            // Glow Gradient
            auto barHeight = juce::jmap(level, 0.0f, 1.0f, 0.0f, bounds.getHeight());
            juce::ColourGradient grad(juce::Colours::aqua.withAlpha(0.6f), x, bounds.getBottom(),
                                      juce::Colours::aqua.withAlpha(0.1f), x, bounds.getY(), false);
            g.setGradientFill(grad);
            g.fillRoundedRectangle(x, bounds.getBottom() - barHeight, meterWidth, barHeight, 3.0f);

            // Peak Hold
            auto peakY = juce::jmap(peak, 0.0f, 1.0f, bounds.getBottom(), bounds.getY());
            g.setColour(juce::Colours::white);
            g.fillRect(x, peakY, meterWidth, 2.0f);
        };

        if (numInputChannels == 1)
        {
            // Mono: center the meter
            drawBar(bounds.getCentreX() - meterWidth * 0.5f, currentLevelL, peakHoldL);
        }
        else
        {
            // Stereo: left & right
            drawBar(bounds.getX() + meterWidth * 0.2f, currentLevelL, peakHoldL);
            drawBar(bounds.getRight() - meterWidth * 1.2f, currentLevelR, peakHoldR);
        }
    }

private:
    void timerCallback() override
    {
        auto smooth = [] (float current, float target)
        {
            return current + (target - current) * 0.2f; // smoothing factor
        };

        currentLevelL = smooth(currentLevelL, targetLevelL);
        currentLevelR = smooth(currentLevelR, targetLevelR);

        // Peak hold decay
        ++peakHoldTime;
        if (peakHoldTime > 30) // hold for 1 sec at 30fps
        {
            peakHoldL *= 0.95f;
            peakHoldR *= 0.95f;
        }

        repaint();
    }

    int numInputChannels = 2;
    float targetLevelL = 0.0f, targetLevelR = 0.0f;
    float currentLevelL = 0.0f, currentLevelR = 0.0f;
    float peakHoldL = 0.0f, peakHoldR = 0.0f;
    int peakHoldTime = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
