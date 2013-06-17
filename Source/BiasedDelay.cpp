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

BiasedDelay::BiasedDelay() : delayBuffer(MAX_CHANNELS, INITIAL_BUFFER_SIZE) {
  parameterNames.add("Time");
  parameterNames.add("Feedback");
  parameterNames.add("Bias");
  parameterNames.add("Dry/Wet");
  
  setParameterValue(PARAMETER_TIME, 0.2f);
  setParameterValue(PARAMETER_FEEDBACK, 0.1f);
  setParameterValue(PARAMETER_BIAS, 0.5f);
  setParameterValue(PARAMETER_DRYWET, 0.5f);
}

/**
 * Processing.
 */

void BiasedDelay::prepareToPlay(double sampleRate, int samplesPerBlock){
  if (this->sampleRate!=sampleRate)
  {
    this->sampleRate = sampleRate;
    delayBuffer.setSize(MAX_CHANNELS, MAX_DELAY * sampleRate, true, false, true);
  }
  writeIdx = 0;
  delayBuffer.clear();
}

void BiasedDelay::processBlock(AudioSampleBuffer& buffer, int numInputChannels,
                               int numOutputChannels, MidiBuffer& midiMessages){
  // Atm we're assuming matching input/output channel counts
  jassert(numInputChannels==numOutputChannels);
  
  for (int channel=0; channel<numInputChannels; channel++)
    processChannelBlock(buffer.getNumSamples(),
                        buffer.getSampleData(channel),
                        delayBuffer.getSampleData(channel));
}

void BiasedDelay::processChannelBlock(int size, float* buf, float* delayBuf){
  unsigned int sampleDelay = getSampleDelay(getParameterValue(PARAMETER_TIME));
  float feedback = getParameterValue(PARAMETER_FEEDBACK);
  float bias = getBiasExponent(1 - getParameterValue(PARAMETER_BIAS));
  float dryWetMix = getParameterValue(PARAMETER_DRYWET);
  
  for (int i=0; i<size; i++)
  {
    float delaySample = delayBuf[writeIdx];
    float v = buf[i] + delaySample * feedback;
    v = applyBias(v, bias);
    delayBuf[writeIdx] = softLimit(v); // Guard: range limit.
    buf[i] = sigmoidXFade(buf[i], delaySample, dryWetMix);
    
    writeIdx = (++writeIdx) % sampleDelay;
  }
}

void BiasedDelay::reset(){
  delayBuffer.clear();
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

/**
 * Mixing.
 */

// Limits to [-1..1] range. This will distort.
float BiasedDelay::hardLimit(float v){
  return fminf(1, fmaxf(-1, v));
}

// Limits to [-1..1] range. A simple/crude "soft" limiter.
float BiasedDelay::softLimit(float v){
  return fminf(1, fmaxf(-1, v));
}

// Linear "X" crossfade from a to b, over mix range [0..1]
float BiasedDelay::linearXFade(float a, float b, float mix){
  return a * (1 - mix) + b * mix;
}

// Sigmoidal "X" crossfade from a to b, over mix range [0..1]
float BiasedDelay::sigmoidXFade(float a, float b, float mix){
  return a * sigmoid(1 - mix) + b * sigmoid(mix);
}

// Linear "transition" crossfade from a to b, over mix range [0..1]
float BiasedDelay::linearTransFade(float a, float b, float mix){
  return a * fminf(1, 2 - 2*mix) + b * fminf(1, 2*mix);
}

// Sigmoidal "transition" crossfade from a to b, over mix range [0..1]
float BiasedDelay::sigmoidTransFade(float a, float b, float mix){
  return a * sigmoid(fminf(1, 2 - 2*mix)) + b * sigmoid(fminf(1, 2*mix));
}

// Mixed curve crossfade from a to b, over mix range [0..1]
// Signal a (dry) is fading out with a sigmoidal curve
// Signal b (wet) is fading in linearly
// This is a compromise: it reduces the dry signal power drop but mostly avoids clipping.
float BiasedDelay::dryWetFade(float a, float b, float mix){
  return a * sigmoid(fminf(1, 2 - 2*mix)) + b * sigmoid(fminf(1, 2*mix));
}

// Returns a sigmoid mapping [0..1] for input values of [0..1]
// Will limit input x to [0..1] range.
float BiasedDelay::sigmoid(float x){
  return sin(fminf(1, fmaxf(0, x)) * M_PI - M_PI_2) / 2 + 0.5f;
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
