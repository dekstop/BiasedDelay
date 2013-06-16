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

#include "BiasedDelay.h"


BiasedDelay::BiasedDelay(){
  parameterNames.add("Time");
  parameterNames.add("Feedback");
  parameterNames.add("Bias");
  parameterNames.add("Dry/Wet");
  
  for (int i=0; i<4; i++)
    setParameterValue(i, 0.0f);
}

BiasedDelay::~BiasedDelay(){
  if (circularBuffer!=NULL)
    delete(circularBuffer);
}


/**
 * Processing.
 */

void BiasedDelay::prepareToPlay(double sampleRate, int samplesPerBlock){
  if (this->sampleRate!=sampleRate){
    if (circularBuffer!=NULL)
      delete(circularBuffer);
    this->sampleRate = sampleRate;
  }
  if (circularBuffer==NULL)
  {
    bufferSize = MAX_DELAY * sampleRate;
    circularBuffer = new float[bufferSize];
    memset(circularBuffer, 0, bufferSize*sizeof(float));
  }
  writeIdx = 0;
}

void BiasedDelay::processBlock(AudioSampleBuffer& buffer, int numInputChannels,
                               int numOutputChannels, MidiBuffer& midiMessages){
  // Atm we're assuming matching input/output channel counts
  jassert(numInputChannels==numOutputChannels);
  
  for (int channel=0; channel<numInputChannels; channel++)
  {
    processChannelBlock(buffer.getSampleData(channel), buffer.getNumSamples());
  }
}

void BiasedDelay::processChannelBlock(float* buf, int size){
  unsigned int sampleDelay = getSampleDelay(getParameterValue(PARAMETER_TIME));
  float feedback = getParameterValue(PARAMETER_FEEDBACK);
  float bias = getBiasExponent(1 - getParameterValue(PARAMETER_BIAS));
  float dryWetMix = getParameterValue(PARAMETER_DRYWET);
  
  for (int i=0; i<size; ++i)
  {
    float delaySample = circularBuffer[writeIdx];
    float v = buf[i] + circularBuffer[writeIdx] * feedback;
    v = applyBias(v, bias);
    circularBuffer[writeIdx] = fminf(1, fmaxf(-1, v)); // Guard: hard range limits.
    buf[i] = linearBlend(buf[i], delaySample, dryWetMix);
    
    writeIdx = (++writeIdx) % sampleDelay;
  }
}

unsigned int BiasedDelay::getSampleDelay(float p1){
  return (MIN_DELAY + p1 * (MAX_DELAY-MIN_DELAY)) * sampleRate;
}

// Mapping p1 parameter ranges so that:
// - full-left (0) is "low bias"
// - centre (0.5) is "no bias"
// - full-right (1.0) is "high bias"
float BiasedDelay::getBiasExponent(float p1){
  if (p1 < 0.5)
  { // min .. med
    p1 = p1 * 2; // [0..1] range
    return p1*p1 * (MED_BIAS-MIN_BIAS) + MIN_BIAS;
  } else
  { // med .. max
    p1 = (p1 - 0.5) * 2; // [0..1] range
    return p1*p1 * (MAX_BIAS-MED_BIAS) + MED_BIAS;
  }
}

float BiasedDelay::applyBias(float v, float bias){
  return
    powf(fabs(v), bias) * // bias
    (v < 0 ? -1 : 1);    // sign
}

// Fade from a to b, over mix range [0..1]
float BiasedDelay::linearBlend(float a, float b, float mix){
  return a * (1 - mix) + b * mix;
}


/**
 * Parameters.
 */

const String BiasedDelay::getParameterName(int index){
  if(index < parameterNames.size())
    return parameterNames[index];
  return String::empty;
}

float BiasedDelay::getParameterValue(int index){
  if(index < sizeof(parameterValues))
    return parameterValues[index];
  return 0.0f;
}

void BiasedDelay::setParameterValue(int index, float value){
  if(index < sizeof(parameterValues))
    parameterValues[index] = value;
}


/**
 * State
 */

XmlElement BiasedDelay::getStateInformation(){
  XmlElement state ("BiasedDelayState");
  for (int i=0; i<getNumParameters(); i++)
    state.setAttribute(getParameterName(i), getParameterValue(i));
  return state;
}

void BiasedDelay::setStateInformation(ScopedPointer<XmlElement> state){
  if (state->hasTagName("BiasedDelayState"))
  {
    for (int i=0; i<getNumParameters(); i++)
      setParameterValue(i, (float)state->getDoubleAttribute(getParameterName(i), getParameterValue(i)));
  }
}
