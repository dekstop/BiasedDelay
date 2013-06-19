/*
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 02110-1301, USA.
 */

/**
 * BiasedDelay.cpp
 * BiasedDelay
 *
 * Martin Dittus 2013-06-16
 * @dekstop
 */

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

const unsigned int MAX_CHANNELS = 4;
const unsigned int INITIAL_BUFFER_SIZE = MAX_DELAY * 44100;

class BiasedDelay {
public:
  BiasedDelay();
  
  // Processing
  void prepareToPlay(double sampleRate, int samplesPerBlock);
  void processBlock(AudioSampleBuffer& buffer, int numInputChannels,
                    int numOutputChannels, MidiBuffer& midiMessages);
  void reset();

  // Parameters
  const float getNumParameters(){return parameterNames.size();};
  const String getParameterName(int index);
  float getParameterValue(int index);
  void setParameterValue(int index, float value);

  // State
  XmlElement getStateInformation();
  void setStateInformation(ScopedPointer<XmlElement> state);

private:
  void processChannelBlock(int size, float* buf, float* delayBuf, int delayBufIdx);
  unsigned int getSampleDelay(float p1);

  float getBiasExponent(float p1);
  float applyBias(float v, float bias);

  // Mixing
  float hardLimit(float v);
  float softLimit(float v);
  float linearXFade(float a, float b, float mix);
  float sigmoidXFade(float a, float b, float mix);
  float linearTransFade(float a, float b, float mix);
  float sigmoidTransFade(float a, float b, float mix);
  float dryWetFade(float a, float b, float mix);
  
  float sigmoid(float x);
  
private:
  StringArray parameterNames;
  float parameterValues[16] = { };
  
  float sampleRate;
  AudioSampleBuffer delayBuffer;
  unsigned int delayBufferIdx;

};

#endif
