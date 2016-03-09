#ifndef __MULTIBANDDISTORTION__
#define __MULTIBANDDISTORTION__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
#include "FFTRect.h"
#include "IPopupMenuControl.h"
#include "ICrossoverControl.h"
#include "RMS.h"
#include "mbprocessing.h"

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
  
  CParamSmooth** mDriveSmoother;
  CParamSmooth** mOutputSmoother;
  CParamSmooth** mMixSmoother;
  

  ISwitchControl* mSoloControl1;
  ISwitchControl* mSoloControl2;
  ISwitchControl* mSoloControl3;
  ISwitchControl* mSoloControl4;

  ISwitchControl* mMuteControl1;
  ISwitchControl* mMuteControl2;
  ISwitchControl* mMuteControl3;
  ISwitchControl* mMuteControl4;
  
  IPopUpMenuControl* mDistMode2;
  IPopUpMenuControl* mDistMode3;
  IPopUpMenuControl* mDistMode4;

  
  //Color Palette
  IColor DARK_GRAY = IColor(255,50,50,50);
  IColor LIGHT_GRAY = IColor(255,70,70,70);
  IColor LIGHT_ORANGE = IColor(255,245,187,0);
  IColor DARK_ORANGE = IColor(255,236,159,5);
  
  
  //RMS level calculator arrays
  RMSFollower** mRMSDry;
  RMSFollower** mRMSWet;
  
  MBProcessing* filterBank;
  
  double* samplesFilteredDry;
  double samplesFilteredWet[4];
  double chebyshev[8];
  double mInputGain;
  double mOutputGain;
  double RMSDry, RMSWet;

  
  const int fftSize=4096;
  const int channelCount = 2;
  
  int mDistType;
  int mPolynomials;

  double mDrive[4];
  double mMix[4];
  int mDistMode[4];
  bool mMute[4];
  bool mSolo[4];
  bool mEnable[4];
  
  bool mDistModesLinked;
  bool mAutoGainComp;
  bool mOutputClipping;
  bool mSpectBypass;

};

#endif
