#include "MultibandDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kDistType = 0,
  kInputGain,
  kOutputGain,
  kAutoGainComp,
  kOutputClipping,
  kNumParams
};

enum ELayout
{

};

MultibandDistortion::MultibandDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mInputGain(0.), mOutputGain(0.)
{
  TRACE;
  
  channelCount=2;
  
  //Initialize Parameters
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDistType)->InitInt("Distortion Type", 1, 1, 8);
  GetParam(kInputGain)->InitDouble("Input Gain", 0., -36., 36., 0.0001, "dB");
  GetParam(kOutputGain)->InitDouble("Output Gain", 0., -36., 36., 0.0001, "dB");
  GetParam(kAutoGainComp)->InitBool("Auto Gain Compensation", true);
  GetParam(kOutputClipping)->InitBool("Output Clipping", false);
  
  //Initialize Parameter Smoothers
  mInputGainSmoother = new CParamSmooth(5.0, GetSampleRate());
  mOutputGainSmoother = new CParamSmooth(5.0, GetSampleRate());

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

MultibandDistortion::~MultibandDistortion() {}


/**
 This is the main loop where we'll process our samples
 */
void MultibandDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.


  for (int i = 0; i < channelCount; i++) {
    double* input = inputs[i];
    double* output = outputs[i];
    
    for (int s = 0; s < nFrames; ++s, ++input, ++output) {
      double sample = *input;
      
      //Apply input gain
      sample *= DBToAmp(mInputGainSmoother->process(mInputGain)); //parameter smoothing prevents artifacts when changing parameter value
      
      /////////////////////////////////////////////////////////////////////
      //Apply distortion
 
      
      //Soft asymmetrical clipping
      //algorithm by Bram de Jong, from musicdsp.org archives
      if (mDistType==1) {
        double threshold = 0.9;
        
        if(sample>threshold)
          sample = threshold + (sample - threshold) / (1 + pow(((sample - threshold)/(1 - threshold)), 2));
        else if(sample >1)
          sample = (sample + 1)/2;
      }
      
      //arctan waveshaper
      else if(mDistType==2){
        double amount = 3;
        sample =  fastAtan(sample * amount);
      }
      
      //Sine shaper
      //based on Jon Watte's waveshaper algorithm. Modified for softer clipping
      else if(mDistType==3){
        double amount = 1.6;
        double z = M_PI * amount/4.0;
        double s = 1/sin(z);
        double b = 1 / amount;
        
        if (sample>b)
          sample = sample + (1-sample)*0.8;
        else if (sample < - b)
          sample = sample + (-1-sample)*0.8;
        else
          sample = sin(z * sample) * s;
        
        sample *= pow(10, -amount/20.0);
      }
      
      //Foldback Distortion
      //algorithm by hellfire@upb.de, from musicdsp.org archives
      else if(mDistType==4){
        double threshold = .9;
        if (sample > threshold || sample < - threshold) {
          sample = fabs(fabs(fmod(sample - threshold, threshold * 4)) - threshold * 2) - threshold;
        }
      }
      
      else if (mDistType==5){
        
      }
      
      else if (mDistType==6){
        
      }
      
      else if (mDistType==7){
        
      }
      
      else if (mDistType==8){
        
      }
      
      //End distortion block
      /////////////////////////////////////////////////////////////////////
      
      
      //Apply output gain
      if (mAutoGainComp) {
        sample *= DBToAmp(mOutputGainSmoother->process(-1*mInputGain));
      }
      else{
        sample *= DBToAmp(mOutputGainSmoother->process(mOutputGain));
      }
      
      
      //Clipping
      if(mOutputClipping){
        if (sample>1) {
          sample = DBToAmp(-0.1);
        }
        else if (sample<-1) {
          sample = -1*DBToAmp(-0.1);
        }
      }
      
      
      *output = sample;
    }
  }
}

void MultibandDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}


void MultibandDistortion::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kDistType:
      mDistType = GetParam(kDistType)->Value();
      break;
      
    case kInputGain:
      mInputGain = GetParam(kInputGain)->Value();
      break;
    
    case kOutputGain:
      mOutputGain = GetParam(kOutputGain)->Value();
      break;
      
    case kAutoGainComp:
      mAutoGainComp=GetParam(kAutoGainComp)->Value();
      break;
      
    case kOutputClipping:
      mOutputClipping = GetParam(kOutputClipping)->Value();
      break;
      
    default:
      break;
  }
}

double MultibandDistortion::fastAtan(double x){
  return (x / (1.0 + 0.28 * (x * x)));
}