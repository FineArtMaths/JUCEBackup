/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BitcrusherAudioProcessorEditor  : 
    public AudioProcessorEditor,
    private ComboBox::Listener,
    private Slider::Listener
{
public:
    BitcrusherAudioProcessorEditor (BitcrusherAudioProcessor&);
    ~BitcrusherAudioProcessorEditor();
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged(Slider* sliderThatHasChanged) override;

    BitcrusherAudioProcessor& processor;
    ComboBox algoChoice;
    Slider sliderA;
    Slider sliderB;
    Slider sliderWDMix;
    Label sliderALabel;
    Label sliderBLabel;
    Label sliderWDMixLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitcrusherAudioProcessorEditor)
};
