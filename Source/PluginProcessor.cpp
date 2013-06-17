/*
  ==============================================================================

  This file was auto-generated!

  It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BiasedDelayAudioProcessor::BiasedDelayAudioProcessor()
{
}

BiasedDelayAudioProcessor::~BiasedDelayAudioProcessor()
{
}

//==============================================================================
const String BiasedDelayAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

int BiasedDelayAudioProcessor::getNumParameters()
{
  return biasedDelay.getNumParameters();
}

float BiasedDelayAudioProcessor::getParameter (int index)
{
  return biasedDelay.getParameterValue(index);
}

void BiasedDelayAudioProcessor::setParameter (int index, float newValue)
{
  biasedDelay.setParameterValue(index, newValue);
}

const String BiasedDelayAudioProcessor::getParameterName (int index)
{
  return biasedDelay.getParameterName(index);
}

const String BiasedDelayAudioProcessor::getParameterText (int index)
{
  return biasedDelay.getParameterName(index);
}

const String BiasedDelayAudioProcessor::getInputChannelName (int channelIndex) const
{
  return String (channelIndex + 1);
}

const String BiasedDelayAudioProcessor::getOutputChannelName (int channelIndex) const
{
  return String (channelIndex + 1);
}

bool BiasedDelayAudioProcessor::isInputChannelStereoPair (int index) const
{
  return true;
}

bool BiasedDelayAudioProcessor::isOutputChannelStereoPair (int index) const
{
  return true;
}

bool BiasedDelayAudioProcessor::acceptsMidi() const
{
  #if JucePlugin_WantsMidiInput
  return true;
  #else
  return false;
  #endif
}

bool BiasedDelayAudioProcessor::producesMidi() const
{
  #if JucePlugin_ProducesMidiOutput
  return true;
  #else
  return false;
  #endif
}

bool BiasedDelayAudioProcessor::silenceInProducesSilenceOut() const
{
  return false;
}

double BiasedDelayAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int BiasedDelayAudioProcessor::getNumPrograms()
{
  return 0;
}

int BiasedDelayAudioProcessor::getCurrentProgram()
{
  return 0;
}

void BiasedDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String BiasedDelayAudioProcessor::getProgramName (int index)
{
  return String::empty;
}

void BiasedDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BiasedDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  biasedDelay.prepareToPlay(sampleRate, samplesPerBlock);
}

void BiasedDelayAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

void BiasedDelayAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  biasedDelay.processBlock(buffer, getNumInputChannels(), getNumOutputChannels(), midiMessages);

  // In case we have more outputs than inputs, we'll clear any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
  {
    buffer.clear (i, 0, buffer.getNumSamples());
  }
}

void BiasedDelayAudioProcessor::reset()
{
  // Use this method as the place to clear any delay lines, buffers, etc, as it
  // means there's been a break in the audio's continuity.
  biasedDelay.reset();
}

//==============================================================================
bool BiasedDelayAudioProcessor::hasEditor() const
{
  return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BiasedDelayAudioProcessor::createEditor()
{
  //  return new BiasedDelayAudioProcessorEditor (this);
  return NULL;
}

//==============================================================================
void BiasedDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  copyXmlToBinary(biasedDelay.getStateInformation(), destData);
}

void BiasedDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
  ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
  if (xmlState != nullptr)
  {
    biasedDelay.setStateInformation(xmlState);
  }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new BiasedDelayAudioProcessor();
}
