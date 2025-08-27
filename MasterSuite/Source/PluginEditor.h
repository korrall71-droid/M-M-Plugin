#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class MasterSuiteAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MasterSuiteAudioProcessorEditor (MasterSuiteAudioProcessor&);
    ~MasterSuiteAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MasterSuiteAudioProcessor& audioProcessor;

    // Simple controls: input/output, basic sections container
    juce::Slider gainInSlider, gainOutSlider;
    juce::Slider eqLo, eqMid, eqHi;
    juce::Slider compThresh, compRatio, compAttack, compRelease, compMakeup;
    juce::Slider satDrive;
    juce::ComboBox satTypeBox;
    juce::Slider delayTime, delayFeedback, delayMix;
    juce::Slider reverbRoom, reverbDamp, reverbWet;
    juce::Slider stereoWidth;
    juce::Slider limThresh;

    // Attachments
    using APVTS = juce::AudioProcessorValueTreeState;
    std::unique_ptr<APVTS::SliderAttachment> att_gain_in, att_gain_out;
    std::unique_ptr<APVTS::SliderAttachment> att_eq_lo, att_eq_mid, att_eq_hi;
    std::unique_ptr<APVTS::SliderAttachment> att_comp_thresh, att_comp_ratio, att_comp_attack, att_comp_release, att_comp_makeup;
    std::unique_ptr<APVTS::SliderAttachment> att_sat_drive;
    std::unique_ptr<APVTS::ComboBoxAttachment> att_sat_type;
    std::unique_ptr<APVTS::SliderAttachment> att_delay_time, att_delay_feedback, att_delay_mix;
    std::unique_ptr<APVTS::SliderAttachment> att_reverb_room, att_reverb_damp, att_reverb_wet;
    std::unique_ptr<APVTS::SliderAttachment> att_stereo_width;
    std::unique_ptr<APVTS::SliderAttachment> att_lim_thresh;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterSuiteAudioProcessorEditor)
};
