#ifndef __MULTIBANDDISTORTION__
#define __MULTIBANDDISTORTION__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
#include "PeakFollower.h"
#include "VAStateVariableFilter.h"
#include "FFTRect.h"

class MultibandDistortion : public IPlug
{
public:
  MultibandDistortion(IPlugInstanceInfo instanceInfo);
  ~MultibandDistortion();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double fastAtan(double x);
private:
  Spect_FFT* sFFT;
  gFFTAnalyzer* gAnalyzer;
  gFFTFreqDraw* gFreqLines;
  CParamSmooth* mInputGainSmoother;
  CParamSmooth* mOutputGainSmoother;
  
  //Set Colors
  IColor DARK_GRAY = IColor(255,31,31,31);
  IColor LIGHT_GRAY = IColor(255,70,70,70);
  IColor LIGHT_ORANGE = IColor(255,245,187,0);
  IColor DARK_ORANGE = IColor(255,236,159,5);
  
  double chebyshev[8];
  double mInputGain;
  double mOutputGain;
  
  double mDrive1;
  double mDrive2;
  double mDrive3;
  double mDrive4;
  
  double mMix1;
  double mMix2;
  double mMix3;
  double mMix4;
  
  
  const int fftSize=4096;
  const int channelCount = 2;
  
  int mDistType;
  int mPolynomials;

  bool mAutoGainComp;
  bool mOutputClipping;
  

  
};

#endif
