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
 * A basic delay effect with sample bias modulation.
 * This can produce great nonlinear saturation effects, however
 * its behaviour is highly dependent on the audio source.
 *
 * TODO:
 * - clear buffer tail when shortening delay time
 *   - OR: always write to full buffer, but stretch it (and adjust read/write speed)
 * - better dry/wet mixing method (don't just fade over)
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
