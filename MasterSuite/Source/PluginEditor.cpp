#include "PluginEditor.h"

MasterSuiteAudioProcessorEditor::MasterSuiteAudioProcessorEditor (MasterSuiteAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (900, 520);

    // Add visible and attachments
    addAndMakeVisible (gainInSlider);
    addAndMakeVisible (gainOutSlider);
    addAndMakeVisible (eqLo); addAndMakeVisible(eqMid); addAndMakeVisible(eqHi);
    addAndMakeVisible (compThresh); addAndMakeVisible(compRatio); addAndMakeVisible(compAttack); addAndMakeVisible(compRelease); addAndMakeVisible(compMakeup);
    addAndMakeVisible (satDrive); addAndMakeVisible (satTypeBox);
    addAndMakeVisible (delayTime); addAndMakeVisible(delayFeedback); addAndMakeVisible(delayMix);
    addAndMakeVisible (reverbRoom); addAndMakeVisible(reverbDamp); addAndMakeVisible(reverbWet);
    addAndMakeVisible (stereoWidth); addAndMakeVisible (limThresh);

    auto& apvts = audioProcessor.getAPVTS();

    att_gain_in = std::make_unique<APVTS::SliderAttachment>(apvts, "gain_in", gainInSlider);
    att_gain_out = std::make_unique<APVTS::SliderAttachment>(apvts, "gain_out", gainOutSlider);

    att_eq_lo = std::make_unique<APVTS::SliderAttachment>(apvts, "eq_lo_gain", eqLo);
    att_eq_mid = std::make_unique<APVTS::SliderAttachment>(apvts, "eq_mid_gain", eqMid);
    att_eq_hi = std::make_unique<APVTS::SliderAttachment>(apvts, "eq_hi_gain", eqHi);

    att_comp_thresh = std::make_unique<APVTS::SliderAttachment>(apvts, "comp_threshold", compThresh);
    att_comp_ratio = std::make_unique<APVTS::SliderAttachment>(apvts, "comp_ratio", compRatio);
    att_comp_attack = std::make_unique<APVTS::SliderAttachment>(apvts, "comp_attack", compAttack);
    att_comp_release = std::make_unique<APVTS::SliderAttachment>(apvts, "comp_release", compRelease);
    att_comp_makeup = std::make_unique<APVTS::SliderAttachment>(apvts, "comp_makeup", compMakeup);

    att_sat_drive = std::make_unique<APVTS::SliderAttachment>(apvts, "sat_drive", satDrive);
    att_sat_type = std::make_unique<APVTS::ComboBoxAttachment>(apvts, "sat_type", satTypeBox);

    att_delay_time = std::make_unique<APVTS::SliderAttachment>(apvts, "delay_time", delayTime);
    att_delay_feedback = std::make_unique<APVTS::SliderAttachment>(apvts, "delay_feedback", delayFeedback);
    att_delay_mix = std::make_unique<APVTS::SliderAttachment>(apvts, "delay_mix", delayMix);

    att_reverb_room = std::make_unique<APVTS::SliderAttachment>(apvts, "reverb_room", reverbRoom);
    att_reverb_damp = std::make_unique<APVTS::SliderAttachment>(apvts, "reverb_damp", reverbDamp);
    att_reverb_wet = std::make_unique<APVTS::SliderAttachment>(apvts, "reverb_wet", reverbWet);

    att_stereo_width = std::make_unique<APVTS::SliderAttachment>(apvts, "stereo_width", stereoWidth);
    att_lim_thresh = std::make_unique<APVTS::SliderAttachment>(apvts, "lim_threshold", limThresh);

    // Basic slider styles
    auto setupSlider = [](juce::Slider& s){ s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20); };
    for (auto* s : { &gainInSlider, &gainOutSlider, &eqLo, &eqMid, &eqHi, &compThresh, &compRatio, &compAttack, &compRelease, &compMakeup, &satDrive, &delayTime, &delayFeedback, &delayMix, &reverbRoom, &reverbDamp, &reverbWet, &stereoWidth, &limThresh })
        setupSlider(*s);

    satTypeBox.addItem("Soft", 1); satTypeBox.addItem("Hard", 2); satTypeBox.addItem("Tube", 3);
    satTypeBox.setSelectedId(1);

    startTimerHz(30); // for repainting meters if needed
}

MasterSuiteAudioProcessorEditor::~MasterSuiteAudioProcessorEditor()
{
}

void MasterSuiteAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB(32,32,40));
    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawFittedText ("MasterSuite — All-in-One Mixing/Mastering", getLocalBounds().removeFromTop(36), juce::Justification::centred, 1);
}

void MasterSuiteAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(12);
    auto top = r.removeFromTop(36);
    auto left = r.removeFromLeft(r.getWidth()/3);

    auto row = left.removeFromTop(120);
    gainInSlider.setBounds(row.removeFromLeft(row.getWidth()/2).reduced(8));
    gainOutSlider.setBounds(row.removeFromLeft(row.getWidth()).reduced(8));

    // EQ
    auto eqArea = r.removeFromTop(140);
    eqLo.setBounds(eqArea.removeFromLeft(120).reduced(8));
    eqMid.setBounds(eqArea.removeFromLeft(120).reduced(8));
    eqHi.setBounds(eqArea.removeFromLeft(120).reduced(8));

    // Compressor block
    auto compArea = r.removeFromTop(140);
    compThresh.setBounds(compArea.removeFromLeft(100).reduced(8));
    compRatio.setBounds(compArea.removeFromLeft(100).reduced(8));
    compAttack.setBounds(compArea.removeFromLeft(100).reduced(8));
    compRelease.setBounds(compArea.removeFromLeft(100).reduced(8));
    compMakeup.setBounds(compArea.removeFromLeft(100).reduced(8));

    // Saturation / Delay / Reverb / Stereo / Limiter
    auto fxArea = r.removeFromTop(160);
    satDrive.setBounds(fxArea.removeFromLeft(100).reduced(8));
    satTypeBox.setBounds(fxArea.removeFromLeft(120).reduced(8));

    delayTime.setBounds(fxArea.removeFromLeft(120).reduced(8));
    delayFeedback.setBounds(fxArea.removeFromLeft(120).reduced(8));
    delayMix.setBounds(fxArea.removeFromLeft(120).reduced(8));

    reverbRoom.setBounds(fxArea.removeFromLeft(120).reduced(8));
    reverbDamp.setBounds(fxArea.removeFromLeft(120).reduced(8));
    reverbWet.setBounds(fxArea.removeFromLeft(120).reduced(8));

    stereoWidth.setBounds(fxArea.removeFromLeft(120).reduced(8));
    limThresh.setBounds(fxArea.removeFromLeft(120).reduced(8));
}
