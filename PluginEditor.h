/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class OverDrive4AudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    OverDrive4AudioProcessorEditor (OverDrive4AudioProcessor&);
    ~OverDrive4AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void roundSliderValue(juce::Slider* slider, float interval);

    OverDrive4AudioProcessor& processor;

    //Sliders
    juce::Slider driveSlider;
    juce::Slider gainSlider;
    juce::Slider mixSlider;
    juce::Slider highPassFreqSlider;
    juce::Slider lowPassFreqSlider;

    //Labels
    juce::Label driveLabel;
    juce::Label gainLabel;
    juce::Label mixLabel;
    juce::Label highPassFreqLabel;
    juce::Label lowPassFreqLabel;
    juce::Label filterLabel;
    juce::Label overdriveLabel;
    
    //Resize Values
    const int margin = 10;
    const int minWidth = 400;
    const int minHeight = 300;
    
    //ValueTreeStates
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassFreqAttachment;
    
    //Rectangle
    juce::Rectangle<int> bottomRectangle;
    
    //UFX Logo
    juce::Image logoImage;
    juce::Rectangle<int> logoBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverDrive4AudioProcessorEditor)
};
