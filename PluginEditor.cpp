/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "underratedFXLookAndFeel.h"
#include "BinaryData.h"  // Include BinaryData

// Create a global instance of the custom LookAndFeel
static UnderratedFXLookAndFeel customLookAndFeel;

OverDrive4AudioProcessorEditor::OverDrive4AudioProcessorEditor (OverDrive4AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    driveSlider.setColour(juce::Slider::thumbColourId, juce::Colours::red); // Example of setting a specific colour
    driveSlider.setLookAndFeel(&customLookAndFeel);
    driveSlider.addListener(this); // Add listener for rounding
    addAndMakeVisible(driveSlider);

    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    gainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setLookAndFeel(&customLookAndFeel);
    gainSlider.addListener(this); // Add listener for rounding
    addAndMakeVisible(gainSlider);

    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    mixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    mixSlider.setTextValueSuffix(" %");
    mixSlider.setValue(50.0f);
    mixSlider.setLookAndFeel(&customLookAndFeel);
    mixSlider.addListener(this); // Add listener for rounding
    addAndMakeVisible(mixSlider);

    highPassFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highPassFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    highPassFreqSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    highPassFreqSlider.setTextValueSuffix(" Hz");
    highPassFreqSlider.setRange(20.0f, 20000.0f, 1.0f); // Set interval to 1 for precise control
    highPassFreqSlider.setSkewFactorFromMidPoint(500.0); // Set skew factor for logarithmic scale
    highPassFreqSlider.setNumDecimalPlacesToDisplay(0);
    highPassFreqSlider.setLookAndFeel(&customLookAndFeel);
    highPassFreqSlider.addListener(this); // Add listener for rounding
    addAndMakeVisible(highPassFreqSlider);

    lowPassFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowPassFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    lowPassFreqSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    lowPassFreqSlider.setTextValueSuffix(" Hz");
    lowPassFreqSlider.setRange(20.0f, 20000.0f, 1.0f); // Set interval to 1 for precise control
    lowPassFreqSlider.setSkewFactorFromMidPoint(500.0); // Set skew factor for logarithmic scale
    lowPassFreqSlider.setLookAndFeel(&customLookAndFeel);
    lowPassFreqSlider.addListener(this); // Add listener for rounding
    addAndMakeVisible(lowPassFreqSlider);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent(&driveSlider, false);
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    driveLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(driveLabel);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    gainLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(gainLabel);

    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    mixLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(mixLabel);

    highPassFreqLabel.setText("HPF Freq", juce::dontSendNotification);
    highPassFreqLabel.attachToComponent(&highPassFreqSlider, false);
    highPassFreqLabel.setJustificationType(juce::Justification::centred);
    highPassFreqLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    highPassFreqLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(highPassFreqLabel);

    lowPassFreqLabel.setText("LPF Freq", juce::dontSendNotification);
    lowPassFreqLabel.attachToComponent(&lowPassFreqSlider, false);
    lowPassFreqLabel.setJustificationType(juce::Justification::centred);
    lowPassFreqLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    lowPassFreqLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(lowPassFreqLabel);

    filterLabel.setText("Filter", juce::dontSendNotification); // Initialize the filter label
    filterLabel.setFont(juce::Font(22.0f, juce::Font::bold)); // Set the font size and style
    filterLabel.setJustificationType(juce::Justification::centred);
    filterLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterLabel);
    
    overdriveLabel.setText("OverDrive", juce::dontSendNotification);
    overdriveLabel.setFont(juce::Font(32.0f, juce::Font::bold));
    overdriveLabel.setJustificationType(juce::Justification::centred);
    overdriveLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(overdriveLabel);
    
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "DRIVE", driveSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "GAIN", gainSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "MIX", mixSlider);
    highPassFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "HPF_FREQ", highPassFreqSlider);
    lowPassFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "LPF_FREQ", lowPassFreqSlider);
    setResizable(true, true);
    //setResizeLimits(minWidth, minHeight, 550, 550);  // Set minimum and maximum sizes
    
    // Load the image from BinaryData
    logoImage = juce::ImageCache::getFromMemory(BinaryData::Colour_ufxlogo_nobackground_png, BinaryData::Colour_ufxlogo_nobackground_pngSize);
    
    //Resizable false
    setSize (600, 400);
    setResizable(false, false);
}

OverDrive4AudioProcessorEditor::~OverDrive4AudioProcessorEditor() {}

void OverDrive4AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB(85, 85, 85));

    // Draw the rounded rectangle outline
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(bottomRectangle.toFloat(), 10.0f, 2.0f); // Line thickness set to 2.0f

    // Draw the logo image
    if (!logoImage.isNull())
    {
        g.drawImage(logoImage, logoBounds.toFloat());
    }

    // Draw the filter background with original color and shadow overlays
    customLookAndFeel.drawFilterBackground(g, bottomRectangle);
}

void OverDrive4AudioProcessorEditor::resized()
{
    driveSlider.setBounds (getWidth() * 0.08, getHeight() * 0.3, 120, 120);
    gainSlider.setBounds (getWidth() * 0.395, getHeight() * 0.20, 120, 120);
    mixSlider.setBounds (getWidth() * 0.72, getHeight() * 0.3, 120, 120);
    highPassFreqSlider.setBounds (getWidth() * 0.33, getHeight() * 0.75, 75, 75);
    lowPassFreqSlider.setBounds (getWidth() * 0.54, getHeight() * 0.75, 75, 75);
     
    // Calculate the bounds for the bottom rectangle
    auto width = getWidth() * 0.4f; // Width increased to 40% of the total width
    auto height = getHeight() * 0.45f; // Height set to 45% of the total height
    auto x = (getWidth() - width) / 2;
    auto y = getHeight() - height - 10;
    bottomRectangle.setBounds(x, y, width, height);
    
    // Set bounds for the filter label
    filterLabel.setBounds(x, y + 10, width, 22);
    overdriveLabel.setBounds((getWidth() / 2) - 85, getHeight() * 0.025, 170, 30);
    
    // Set bounds for the logo image in the top corner
    logoBounds.setBounds(5, 5, 130, 40); // Adjust the size and position as needed
}

void OverDrive4AudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &driveSlider)
    {
        roundSliderValue(slider, 0.5f);
    }
    else if (slider == &gainSlider)
    {
        roundSliderValue(slider, 0.1f);
    }
    else if (slider == &mixSlider)
    {
        roundSliderValue(slider, 0.5f);
    }
    else if (slider == &highPassFreqSlider || slider == &lowPassFreqSlider)
    {
        roundSliderValue(slider, 10.0f);
    }
}

void OverDrive4AudioProcessorEditor::roundSliderValue(juce::Slider* slider, float interval)
{
    float value = slider->getValue();
    float roundedValue = std::round(value / interval) * interval;
    slider->setValue(roundedValue, juce::dontSendNotification);
}
