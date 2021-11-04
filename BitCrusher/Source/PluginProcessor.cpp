/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrusherAudioProcessor::BitcrusherAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    softRegisters = new float[10];
}

BitcrusherAudioProcessor::~BitcrusherAudioProcessor()
{
}

//==============================================================================
const String BitcrusherAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BitcrusherAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BitcrusherAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BitcrusherAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BitcrusherAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BitcrusherAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BitcrusherAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BitcrusherAudioProcessor::setCurrentProgram (int index)
{
}

const String BitcrusherAudioProcessor::getProgramName (int index)
{
    return {};
}

void BitcrusherAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BitcrusherAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BitcrusherAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BitcrusherAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BitcrusherAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {

            auto input = channelData[i];
            auto cleanOut = channelData[i];
            if (algorithm == 1) {
                /* Traditional bit crusher

                   Variables:
                		paramA is the sample reduction amount
                		paramB is the bit depth
                		softRegisters[0] is the sample we're holding onto
                		softRegisters[1] counts how long we've held it
                		softRegisters[2] is the number of samples to hold for
                		softRegisters[3] is derived from the bit depth
                	Bit Rate Reduction Algorithm:
                		softRegisters[2] = floorf(paramA * 99) + 1, a whole number between 1 and 100
                		softRegisters[1] increments each sample.
                			When it hits softRegisters[2], reset it and grab a new sample into softRegisters[0]
                		Always output softRegisters[0]
                	Bit depth Reduction Algorithm
                		The input is the output of the previous algo
                		softRegisters[3] = pow(2, floorf(paramA * 10) + 1), a power of 2 between 2 and 2^10
                			(might want to go higher than 10 -- maybe up to 32 or even 64 for a 64-bit system makes sense)
                		Output floorf(softRegisters[0] * softRegisters[3]) / softRegisters[3]
                */

                if (SOFT_REG_NULL_VALUE == softRegisters[1]
                    || softRegisters[1] >= softRegisters[2]) {
                    softRegisters[1] = 0.0f;
                    softRegisters[0] = floorf(input * softRegisters[3]) / softRegisters[3];
                }
                input = softRegisters[0];
                softRegisters[1]++;
            } else if (algorithm == 2) {
                /* Amplitude modulation
                     softRegisters[0] is 1 or -1, rising or falling
                     softRegisters[1] counts samples during rise or fall
                     softRegisters[2] is number of samples in a rise or fall cycle 
                        (between 10 and 20000)
                     softRegisters[3] is shape (between -5 and 5, not including 0)
                */
                if (SOFT_REG_NULL_VALUE == softRegisters[1]
                    || softRegisters[1] >= softRegisters[2]) {
                    softRegisters[1] = 0.0f;
                    softRegisters[0] *= -1;
                }
                float progress = softRegisters[1] / softRegisters[2];
                if (softRegisters[0] < 0) {
                    progress = 1 - progress;
                }
                input *= 2*(pow(progress, softRegisters[3])) - 1;
                softRegisters[1]++;
            }
            else if (algorithm == 3) {
                /*
                    Clamp Out
                        If the sample is outside the threshold value,
                        clamp it to a single value with the sign of the sample.
                    paramA = threshold
                    paramB = clip value
                */
                if (input > softRegisters[0]) {
                    input = softRegisters[1];
                }
                else if(input < -1* softRegisters[0]){
                    input = -1 * softRegisters[1];
                }
            }
            else if (algorithm == 4) {
                /*
                    Clamp in
                        If the sample is inside the threshold value,
                        clamp it to a single value with the sign of the sample.
                    paramA = threshold
                    paramB = clip value
                */
                if (input > 0 && input < softRegisters[0]) {
                    input = softRegisters[1];
                }
                else if (input < 0  && input > -1 * softRegisters[0]) {
                    input = -1 * softRegisters[1];
                }
            } else if (algorithm == 5) {
                /*
                Absolute Value
                    paramA sets the number of samples to flip, then the number to leave unchanged
                    paramB sets a threshold for flipping

                    softRegisters[0] counts samples
                    softRegisters[1] is the number of samples to wait before flipping mode
                    softRegisters[2] is the current mode
                    softRegisters[3] is the threshold
                */
                softRegisters[0]++;
                if (softRegisters[0] >= softRegisters[1]) {
                    softRegisters[2] = -1 * softRegisters[2];
                    softRegisters[0] = 0;
                }
                if (softRegisters[2] > 0 &&
                    abs(input) > softRegisters[3]) {
                    input = abs(input) * 2 - 1;
                }
            }
            channelData[i] = ((1 - wetDryMix) * cleanOut) + (wetDryMix * input);
        }

    }


}

//==============================================================================
bool BitcrusherAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BitcrusherAudioProcessor::createEditor()
{
    return new BitcrusherAudioProcessorEditor (*this);
}

//==============================================================================
void BitcrusherAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BitcrusherAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void BitcrusherAudioProcessor::changeParamA(float value) {
    paramA = value;
    if (algorithm == 1) {
        softRegisters[2] = floorf(paramA * 99) + 1;
    } else if (algorithm == 2) {
        softRegisters[2] = floorf(500 * (1 - paramA)) + 10;
    } else if (algorithm == 3 || algorithm == 4) {
        softRegisters[0] = paramA/3.0f;
    } else if (algorithm == 5) {
        softRegisters[1] = paramA * 5000;
    }
}

void BitcrusherAudioProcessor::changeParamB(float value) {
    paramB = value;
    if (algorithm == 1) {
        softRegisters[3] = pow(2, floorf(paramB * 64 + 1));
    } else if (algorithm == 2) {
        softRegisters[3] = floorf(paramB * 2) + 1;
    } else if (algorithm == 3 || algorithm == 4) {
        softRegisters[1] = paramB / 3.0f;
    } else if (algorithm == 5) {
        softRegisters[3] = paramB / 3.0f;
    }
}

void BitcrusherAudioProcessor::changeAlgorithm(int algo) {
    algorithm = algo;
    for (int i = 0; i < sizeof(softRegisters); ++i) {
        softRegisters[i] = SOFT_REG_NULL_VALUE;
    }
    // Populate any softRegisters that need to be set in advance
    changeParamA(paramA);
    changeParamB(paramA);
}
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BitcrusherAudioProcessor();
}

