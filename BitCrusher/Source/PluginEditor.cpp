/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrusherAudioProcessorEditor::BitcrusherAudioProcessorEditor (BitcrusherAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible(&algoChoice);
    algoChoice.addItem("Bitcrusher", 1);
    algoChoice.addItem("Ring Modulator", 2);
    algoChoice.addItem("Clamp Out", 3);
    algoChoice.addItem("Clamp In", 4);
    algoChoice.addItem("Absolute Value", 5);
    algoChoice.setSelectedId(1);
    algoChoice.addListener(this);

    addAndMakeVisible(&sliderA);
    sliderA.setRange(0.0f, 1.0f, 0.001f);
    sliderA.addListener(this);
    addAndMakeVisible(&sliderB);
    sliderB.setRange(0.0f, 1.0f, 0.001f);
    sliderB.addListener(this);
    addAndMakeVisible(&sliderWDMix);
    sliderWDMix.setRange(0.0f, 1.0f, 0.001f);
    sliderWDMix.addListener(this);
    addAndMakeVisible(&sliderALabel);
    addAndMakeVisible(&sliderBLabel);
    addAndMakeVisible(&sliderWDMixLabel);
    sliderWDMixLabel.setText("Wet/Dry", NotificationType::dontSendNotification);

    comboBoxChanged(&algoChoice);   // sets the labels for soft controls
}

BitcrusherAudioProcessorEditor::~BitcrusherAudioProcessorEditor()
{
}

//==============================================================================
void BitcrusherAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void BitcrusherAudioProcessorEditor::resized()
{
    algoChoice.setBounds(50, 50, 200, 50);
    sliderA.setBounds(50, 100, 200, 50);
    sliderALabel.setBounds(255, 100, 50, 50);
    sliderB.setBounds(50, 150, 200, 50);
    sliderBLabel.setBounds(255, 150, 50, 50);
    sliderWDMix.setBounds(50, 200, 200, 50);
    sliderWDMixLabel.setBounds(255, 200, 50, 50);
}

void BitcrusherAudioProcessorEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged) {
    int id = comboBoxThatHasChanged->getSelectedId();
    processor.changeAlgorithm(id);
    if (id == 1) {
        sliderALabel.setText("Rate", NotificationType::dontSendNotification);
        sliderBLabel.setText("Depth", NotificationType::dontSendNotification);
    } else if (id == 2) {
        sliderALabel.setText("Freq", NotificationType::dontSendNotification);
        sliderBLabel.setText("Shape", NotificationType::dontSendNotification);
    }
    else if (id == 3 || id == 4) {
        sliderALabel.setText("Threshold", NotificationType::dontSendNotification);
        sliderBLabel.setText("Clamp", NotificationType::dontSendNotification);
    }
    else if (id == 5) {
        sliderALabel.setText("Threshold", NotificationType::dontSendNotification);
        sliderBLabel.setText("Skip Rate", NotificationType::dontSendNotification);
    }

}

void BitcrusherAudioProcessorEditor::sliderValueChanged(Slider* sliderThatHasChanged) {
    if (&sliderA == sliderThatHasChanged) {
        processor.changeParamA(sliderThatHasChanged->getValue());
    } else if (&sliderB == sliderThatHasChanged) {
        processor.changeParamB(sliderThatHasChanged->getValue());
    }
    else if (&sliderWDMix == sliderThatHasChanged) {
        processor.wetDryMix = sliderThatHasChanged->getValue();
    }
}
