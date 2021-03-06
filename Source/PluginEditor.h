/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINEDITOR_H_76923914__
#define __PLUGINEDITOR_H_76923914__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class BiasedDelayAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    BiasedDelayAudioProcessorEditor (BiasedDelayAudioProcessor* ownerFilter);
    ~BiasedDelayAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
};


#endif  // __PLUGINEDITOR_H_76923914__
