/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

OverDrive4AudioProcessor::OverDrive4AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                       ),
                       parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

OverDrive4AudioProcessor::~OverDrive4AudioProcessor() {}

const juce::String OverDrive4AudioProcessor::getName() const { return JucePlugin_Name; }

bool OverDrive4AudioProcessor::acceptsMidi() const { return false; }
bool OverDrive4AudioProcessor::producesMidi() const { return false; }
bool OverDrive4AudioProcessor::isMidiEffect() const { return false; }
double OverDrive4AudioProcessor::getTailLengthSeconds() const { return 0.0; }

int OverDrive4AudioProcessor::getNumPrograms() { return 1; }
int OverDrive4AudioProcessor::getCurrentProgram() { return 0; }
void OverDrive4AudioProcessor::setCurrentProgram (int index) {}
const juce::String OverDrive4AudioProcessor::getProgramName (int index) { return {}; }
void OverDrive4AudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void OverDrive4AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    highPassFilter.prepare(spec);
    highPassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highPassFilter.setCutoffFrequency(20.0f); // Initial value
    highPassFilter.setResonance(0.7f); // Lower resonance for gentler slope

    lowPassFilter.prepare(spec);
    lowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lowPassFilter.setCutoffFrequency(20000.0f); // Initial value
    lowPassFilter.setResonance(0.7f); // Lower resonance for gentler slope
}

void OverDrive4AudioProcessor::releaseResources() {}

void OverDrive4AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto drive = parameters.getRawParameterValue("DRIVE")->load();
    auto gain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("GAIN")->load());
    auto mix = parameters.getRawParameterValue("MIX")->load() / 100.0f; // Convert mix to 0.0 - 1.0 range

    auto highPassFreq = parameters.getRawParameterValue("HPF_FREQ")->load();
    auto lowPassFreq = parameters.getRawParameterValue("LPF_FREQ")->load();

    highPassFreq = std::round(highPassFreq / 10.0f) * 10.0f;
    lowPassFreq = std::round(lowPassFreq / 10.0f) * 10.0f;

    highPassFilter.setCutoffFrequency(highPassFreq);
    lowPassFilter.setCutoffFrequency(lowPassFreq);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    juce::AudioBuffer<float> wetBuffer;
    wetBuffer.makeCopyOf(buffer);

    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        auto* channelData = wetBuffer.getWritePointer (channel);

        for (int sample = 0; sample < wetBuffer.getNumSamples(); ++sample)
        {
            // Apply overdrive effect
            float cleanSignal = channelData[sample];
            float overdrivenSignal = std::tanh(drive * cleanSignal) * gain;

            channelData[sample] = overdrivenSignal;
        }
    }

    // Apply high-pass filter
    juce::dsp::AudioBlock<float> wetBlock(wetBuffer);
    juce::dsp::ProcessContextReplacing<float> wetContext(wetBlock);
    highPassFilter.process(wetContext);

    // Apply low-pass filter
    lowPassFilter.process(wetContext);

    // Apply wet/dry mix
    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        auto* dryChannelData = buffer.getWritePointer(channel);
        auto* wetChannelData = wetBuffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            dryChannelData[sample] = dryChannelData[sample] * (1.0f - mix) + wetChannelData[sample] * mix;
        }
    }
}

bool OverDrive4AudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* OverDrive4AudioProcessor::createEditor() { return new OverDrive4AudioProcessorEditor (*this); }

void OverDrive4AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    parameters.state.writeToStream(stream);
}

void OverDrive4AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        parameters.state = tree;
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout OverDrive4AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive", 0.0f, 10.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("HPF_FREQ", "High-Pass Frequency", 20.0f, 20000.0f, 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LPF_FREQ", "Low-Pass Frequency", 20.0f, 20000.0f, 20000.0f));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OverDrive4AudioProcessor();
}
