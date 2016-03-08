#ifndef __MULTIBANDDISTORTION__
#define __MULTIBANDDISTORTION__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
#include "FFTRect.h"
#include "IPopupMenuControl.h"

class MultibandDistortion : public IPlug
{
public:
  MultibandDistortion(IPlugInstanceInfo instanceInfo);
  ~MultibandDistortion();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double fastAtan(double x);
  double ProcessDistortion(double sample, int distType);

private:
  Spect_FFT* sFFT;
  gFFTAnalyzer* gAnalyzer;
  gFFTFreqDraw* gFreqLines;
  CParamSmooth* mInputGainSmoother;
  CParamSmooth* mOutputGainSmoother;
  
  //Set Colors
  IColor DARK_GRAY = IColor(255,50,50,50);
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

  int mDistMode1;
  int mDistMode2;
  int mDistMode3;
  int mDistMode4;
  
  bool mAutoGainComp;
  bool mOutputClipping;
  
  bool mSpectBypass;
  
  bool mBand1Bypass;
  bool mBand2Bypass;
  bool mBand3Bypass;
  bool mBand4Bypass;

};

#endif
