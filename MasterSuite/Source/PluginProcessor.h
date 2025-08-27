#pragma once
#include <JuceHeader.h>

class MasterSuiteAudioProcessor  : public juce::AudioProcessor
{
public:
    MasterSuiteAudioProcessor();
    ~MasterSuiteAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MasterSuite"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Simple metering (atomics updated in processBlock)
    std::atomic<float> peakLeft {0.0f}, peakRight {0.0f};
    std::atomic<float> rmsLeft {0.0f}, rmsRight {0.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterSuiteAudioProcessor)
};
