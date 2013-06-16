//
//  BiasedDelay.h
//  BiasedDelay
//
//  A basic delay effect with sample bias modulation.
//  This can produce great nonlinear saturation effects, however
//  its behaviour is highly dependent on the audio source.
//
//  Created by mongo on 16/06/2013.
//
//

#ifndef BiasedDelay_BiasedDelay_h
#define BiasedDelay_BiasedDelay_h

#include "../JuceLibraryCode/JuceHeader.h"

enum ParameterId {
  PARAMETER_TIME = 0,
  PARAMETER_FEEDBACK,
  PARAMETER_BIAS,
  PARAMETER_DRYWET
};

const float MIN_DELAY = 0.01; // in seconds
const float MAX_DELAY = 4;

const float MIN_BIAS = 0.5;
const float MED_BIAS = 1;
const float MAX_BIAS = 3;


class BiasedDelay {
public:
  BiasedDelay();
  ~BiasedDelay();
  
  // Processing
  void prepareToPlay(double sampleRate, int samplesPerBlock);
  void processBlock(AudioSampleBuffer& buffer, int numInputChannels,
                    int numOutputChannels, MidiBuffer& midiMessages);

  // Parameters
  const float getNumParameters(){return parameterNames.size();};
  const String getParameterName(int index);
  float getParameterValue(int index);
  void setParameterValue(int index, float value);

  // State
  XmlElement getStateInformation();
  void setStateInformation(ScopedPointer<XmlElement> state);

private:
  void processChannelBlock(float* data, int numSamples);
  unsigned int getSampleDelay(float p1);
  float getBiasExponent(float p1);
  float applyBias(float v, float bias);
  float linearBlend(float a, float b, float mix);
  
private:
  StringArray parameterNames;
  float parameterValues[16];
  
  float sampleRate;
  unsigned int bufferSize;
  float* circularBuffer;
  unsigned int writeIdx;

};


#endif
