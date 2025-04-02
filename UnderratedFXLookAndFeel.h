/*
  ==============================================================================

    UnderratedFXLookAndFeel.h
    Created: 29 Jul 2024 7:18:29pm
    Author:  Bradley Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class UnderratedFXLookAndFeel : public juce::LookAndFeel_V4
{
public:
    UnderratedFXLookAndFeel()
    {
        // Colours to set across the plugin
        // Sliders
        setColour(juce::Slider::thumbColourId, pointerColour); // Use the global pointer color
        setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(0, 255, 255));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(0, 255, 255));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB(0, 255, 255));
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
        setColour(juce::Slider::textBoxTextColourId, juce::Colour::fromRGB(0, 255, 255));
        
        // Labels
        setColour(juce::Label::textColourId, juce::Colour::fromRGB(0, 255, 255));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Draw 3D shadow for base circle
        g.setColour(shadowColour.withAlpha(0.3f));
        g.fillEllipse(rx + 2.0f, ry + 2.0f, rw, rw);

        // Fill base circle with gradient for 3D effect
        juce::ColourGradient gradient(slider.findColour(juce::Slider::rotarySliderFillColourId).brighter(0.1f),
                                      centreX, centreY - radius,
                                      slider.findColour(juce::Slider::rotarySliderFillColourId).darker(0.1f),
                                      centreX, centreY + radius,
                                      false);
        g.setGradientFill(gradient);
        g.fillEllipse(rx, ry, rw, rw);

        // Draw outline with shadow
        g.setColour(shadowColour.withAlpha(0.5f)); // Use global shadow color
        g.drawEllipse(rx - 2, ry - 2, rw + 4, rw + 4, 2.0f); // Draw shadow

        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawEllipse(rx, ry, rw, rw, 1.5f);

        // Add highlights for a more 3D look
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.drawEllipse(rx + 1, ry + 1, rw - 2, rw - 2, 2.0f);

        juce::Path p;
        auto pointerLength = radius; // Pointer length to reach the edge
        auto pointerThickness = 4.0f; // Thicker pointer
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // Draw pointer with shadow
        g.setColour(shadowColour.withAlpha(0.5f)); // Use global shadow color
        g.fillPath(p, juce::AffineTransform::translation(2.0f, 2.0f)); // Draw shadow

        // Draw pointer
        g.setColour(pointerColour); // Use global pointer color
        g.fillPath(p);
    }

    void drawInnerShadow(juce::Graphics& g, juce::Rectangle<float> bounds, float cornerSize, float shadowSize)
    {
        for (float i = 0; i < shadowSize; ++i)
        {
            g.setColour(juce::Colour::fromRGB(250, 250, 250).withAlpha(0.2f * (shadowSize - i)));
            g.drawRoundedRectangle(bounds.reduced(i), cornerSize - i, 1.0f);
        }
    }

    void drawFilterBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        auto innerBounds = bounds.toFloat().reduced(4.0f);
        auto cornerSize = 10.0f;  // Radius for the rounded corners
        auto shadowSize = 3.0f;   // Size of the inner shadow

        // Draw main background with transparent color
        g.setColour(juce::Colours::transparentWhite);
        g.fillRoundedRectangle(innerBounds, cornerSize);

        // Draw inner shadow
        drawInnerShadow(g, innerBounds, cornerSize, shadowSize);

        // Draw transparent inset border to enhance 3D effect
        g.setColour(juce::Colours::transparentBlack.withAlpha(0.5f));
        g.drawRoundedRectangle(innerBounds, cornerSize, 1.0f);
    }

    static juce::Colour shadowColour;
    static juce::Colour pointerColour;
};

// Define the static member variables
juce::Colour UnderratedFXLookAndFeel::shadowColour = juce::Colour::fromRGB(90, 90, 90);
juce::Colour UnderratedFXLookAndFeel::pointerColour = juce::Colour::fromRGB(85, 85, 85);

