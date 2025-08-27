#include "PluginProcessor.h"
#include "PluginEditor.h"

// Simple helper: linear dB <-> gain
static float dbToGain(float db) { return std::pow(10.0f, db / 20.0f); }

MasterSuiteAudioProcessor::MasterSuiteAudioProcessor()
 : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                   .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
   apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
}

MasterSuiteAudioProcessor::~MasterSuiteAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MasterSuiteAudioProcessor::createParameterLayout()
{
    using P = juce::AudioProcessorValueTreeState;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Gain staging
    params.push_back (std::make_unique<juce::AudioParameterFloat>("gain_in","Input Gain", -24.0f, 24.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("gain_out","Output Gain", -24.0f, 24.0f, 0.0f));

    // Simple 3-band EQ (gain in dB)
    params.push_back (std::make_unique<juce::AudioParameterFloat>("eq_lo_gain","Low Gain", -12.0f, 12.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("eq_mid_gain","Mid Gain", -12.0f, 12.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("eq_hi_gain","High Gain", -12.0f, 12.0f, 0.0f));

    // Compressor (simple)
    params.push_back (std::make_unique<juce::AudioParameterFloat>("comp_threshold","Comp Threshold", -60.0f, 0.0f, -12.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("comp_ratio","Comp Ratio", 1.0f, 20.0f, 2.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("comp_attack","Comp Attack (ms)", 0.1f, 200.0f, 10.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("comp_release","Comp Release (ms)", 10.0f, 2000.0f, 100.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("comp_makeup","Comp Makeup (dB)", -12.0f, 12.0f, 0.0f));

    // Saturation / Distortion
    params.push_back (std::make_unique<juce::AudioParameterFloat>("sat_drive","Sat Drive", 0.0f, 30.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterChoice>("sat_type","Sat Type", juce::StringArray({"Soft","Hard","Tube"}), 0));

    // Delay
    params.push_back (std::make_unique<juce::AudioParameterFloat>("delay_time","Delay Time (ms)", 1.0f, 2000.0f, 350.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("delay_feedback","Delay Feedback", 0.0f, 0.99f, 0.45f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("delay_mix","Delay Mix", 0.0f, 1.0f, 0.25f));

    // Reverb
    params.push_back (std::make_unique<juce::AudioParameterFloat>("reverb_room","Reverb Room Size", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("reverb_damp","Reverb Damping", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("reverb_wet","Reverb Wet", 0.0f, 1.0f, 0.15f));

    // Stereo width
    params.push_back (std::make_unique<juce::AudioParameterFloat>("stereo_width","Stereo Width", 0.0f, 2.0f, 1.0f));

    // Limiter
    params.push_back (std::make_unique<juce::AudioParameterFloat>("lim_threshold","Limiter Threshold (dB)", -24.0f, 0.0f, -0.1f));

    return { params.begin(), params.end() };
}

void MasterSuiteAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<uint32> (samplesPerBlock), static_cast<uint32> (getTotalNumInputChannels()) };

    // EQ filters
    for (int ch=0; ch<2; ++ch)
    {
        // nothing per-channel kept in this simple example
    }

    // Initialize DSP objects
    // Compressor
    comp.reset();
    comp.setSampleRate(sampleRate);
    comp.setAttack(10.0f);
    comp.setRelease(100.0f);

    // Reverb
    reverb.reset();
    juce::dsp::Reverb::Parameters rp;
    rp.damping = 0.5f;
    rp.roomSize = 0.5f;
    rp.wetLevel = 0.15f;
    reverb.setParameters(rp);

    // Delay buffer
    delayBuffer.setSize (2, (int)(sampleRate * 5.0) + samplesPerBlock); // up to 5 seconds
    delayWritePos = 0;

    // Oversimplified: nothing else to init here
}

void MasterSuiteAudioProcessor::releaseResources() {}

bool MasterSuiteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

// --- Simple internal DSP objects declared as static locals to keep file compact ---
struct SimpleCompressor
{
    juce::dsp::Compressor<float> dsp;
    void setParams(float thresholdDb, float ratio, float attackMs, float releaseMs, float makeupDb)
    {
        dsp.setThreshold(thresholdDb);
        dsp.setRatio(ratio);
        dsp.setAttack(attackMs);
        dsp.setRelease(releaseMs);
    }
    void process(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        dsp.process(ctx);
    }
} comp;

juce::dsp::Reverb reverb;
juce::AudioBuffer<float> delayBuffer;
int delayWritePos = 0;

// simple biquad EQ using IIR filters (3 bands)
juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelf, midPeak, highShelf;

// Simple waveshaper for saturation
std::function<float(float)> makeWaveshaper(float drive, int type)
{
    // type: 0 soft, 1 hard, 2 tube-like
    return [drive, type](float x)->float {
        x *= (1.0f + drive * 0.03f);
        if (type == 0) // soft tanh
            return std::tanh(x);
        else if (type == 1) // hard clip
            return std::max(-1.0f, std::min(1.0f, x));
        else // tube-ish
            return (std::tanh(x) + 0.2f * x) / 1.2f;
    };
}

void MasterSuiteAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    auto gainInDb = apvts.getRawParameterValue("gain_in")->load();
    auto gainOutDb = apvts.getRawParameterValue("gain_out")->load();

    float gainIn = dbToGain(gainInDb);
    float gainOut = dbToGain(gainOutDb);

    buffer.applyGain(gainIn);

    // --- EQ (3 bands) ---
    float loGainDb = apvts.getRawParameterValue("eq_lo_gain")->load();
    float midGainDb = apvts.getRawParameterValue("eq_mid_gain")->load();
    float hiGainDb = apvts.getRawParameterValue("eq_hi_gain")->load();

    auto sampleRate = getSampleRate();
    // configure filters per block (cheap but simple)
    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0, 0.7071f, dbToGain(loGainDb));
    *midPeak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0, 0.7071f, dbToGain(midGainDb));
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 8000.0, 0.7071f, dbToGain(hiGainDb));

    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        lowShelf.process(ctx);
        midPeak.process(ctx);
        highShelf.process(ctx);
    }

    // --- Compressor ---
    float compThresh = apvts.getRawParameterValue("comp_threshold")->load();
    float compRatio = apvts.getRawParameterValue("comp_ratio")->load();
    float compAttack = apvts.getRawParameterValue("comp_attack")->load();
    float compRelease = apvts.getRawParameterValue("comp_release")->load();
    float compMakeup = apvts.getRawParameterValue("comp_makeup")->load();

    comp.setParams(compThresh, compRatio, compAttack, compRelease, compMakeup);
    comp.process(buffer);

    // --- Saturation ---
    float drive = apvts.getRawParameterValue("sat_drive")->load();
    int satType = static_cast<int>(apvts.getRawParameterValue("sat_type")->load());
    auto waveshaper = makeWaveshaper(drive, satType);

    for (int ch=0; ch<numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i=0;i<numSamples;++i)
            data[i] = waveshaper(data[i]);
    }

    // --- Delay (simple circular buffer) ---
    float delayMs = apvts.getRawParameterValue("delay_time")->load();
    float feedback = apvts.getRawParameterValue("delay_feedback")->load();
    float mix = apvts.getRawParameterValue("delay_mix")->load();

    int delaySamples = (int)(getSampleRate() * (delayMs / 1000.0f));

    for (int ch=0; ch<numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto* db = delayBuffer.getWritePointer(juce::jmin(ch, delayBuffer.getNumChannels()-1));
        int readPos = (delayWritePos + delayBuffer.getNumSamples() - delaySamples) % delayBuffer.getNumSamples();

        for (int i=0;i<numSamples;++i)
        {
            float in = data[i];
            float delayed = db[readPos];
            float out = in * (1.0f - mix) + delayed * mix;
            data[i] = out;
            // write into delay buffer
            db[delayWritePos] = in + delayed * feedback;
            if (++readPos >= delayBuffer.getNumSamples()) readPos = 0;
            if (++delayWritePos >= delayBuffer.getNumSamples()) delayWritePos = 0;
        }
    }

    // --- Reverb (JUCE dsp) ---
    juce::dsp::AudioBlock<float> reverbBlock (buffer);
    juce::dsp::ProcessContextReplacing<float> reverbCtx (reverbBlock);
    auto rvRoom = apvts.getRawParameterValue("reverb_room")->load();
    auto rvDamp = apvts.getRawParameterValue("reverb_damp")->load();
    auto rvWet = apvts.getRawParameterValue("reverb_wet")->load();
    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = rvRoom;
    rp.damping = rvDamp;
    rp.wetLevel = rvWet;
    rp.dryLevel = 1.0f - rvWet;
    reverb.setParameters(rp);
    reverb.process(reverbCtx);

    // --- Stereo width (mid/side) ---
    float width = apvts.getRawParameterValue("stereo_width")->load(); // 0..2
    if (numChannels >= 2)
    {
        auto* L = buffer.getWritePointer(0);
        auto* R = buffer.getWritePointer(1);
        for (int i=0;i<numSamples;++i)
        {
            float mid = 0.5f * (L[i] + R[i]);
            float side = 0.5f * (L[i] - R[i]);
            side *= width;
            L[i] = mid + side;
            R[i] = mid - side;
        }
    }

    // --- Limiter (simple soft clip to threshold) ---
    float limThreshDb = apvts.getRawParameterValue("lim_threshold")->load();
    float limThresh = dbToGain(limThreshDb);
    for (int ch=0; ch<numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i=0;i<numSamples;++i)
        {
            if (data[i] > limThresh) data[i] = limThresh + (data[i]-limThresh)*0.001f;
            if (data[i] < -limThresh) data[i] = -limThresh + (data[i]+limThresh)*0.001f;
        }
    }

    // Output gain
    buffer.applyGain(gainOut);

    // --- Simple metering (peak & RMS) ---
    float peakL = 0.0f, peakR = 0.0f, sumL = 0.0f, sumR = 0.0f;
    for (int i=0;i<numSamples;++i)
    {
        if (numChannels>0) { float v = std::abs(buffer.getReadPointer(0)[i]); peakL = std::max(peakL, v); sumL += v*v; }
        if (numChannels>1) { float v = std::abs(buffer.getReadPointer(1)[i]); peakR = std::max(peakR, v); sumR += v*v; }
    }
    rmsLeft.store (std::sqrt(sumL / (float)numSamples));
    rmsRight.store (std::sqrt(sumR / (float)numSamples));
    peakLeft.store (peakL);
    peakRight.store (peakR);
}

void MasterSuiteAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }
}

void MasterSuiteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// Create plugin filter
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MasterSuiteAudioProcessor();
}
