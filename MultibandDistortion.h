#ifndef __MULTIBANDDISTORTION__
#define __MULTIBANDDISTORTION__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
#include "FFTRect.h"
#include "IPopupMenuControl.h"
#include "ICrossoverControl.h"
#include "RMS.h"
#include "LinkwitzRiley.h"
#include "CFxRbjFilter.h"
#include "PeakFollower.h"

#define WDL_BESSEL_FILTER_ORDER 8
#define WDL_BESSEL_DENORMAL_AGGRESSIVE
#include "besselfilter.h"

class MultibandDistortion : public IPlug
{
public:
  MultibandDistortion(IPlugInstanceInfo instanceInfo);
  ~MultibandDistortion();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  double ProcessDistortion(double sample, int distType);
  
private:
  void smoothFilters();
  double fastAtan(double x);
  double percentToFreq(double p);

  Spect_FFT* sFFT;
  gFFTAnalyzer* gAnalyzer;
  gFFTFreqDraw* gFreqLines;
  CParamSmooth mInputGainSmoother;
  CParamSmooth mOutputGainSmoother;
  
  CParamSmooth mDriveSmoother[4];
  CParamSmooth mOutputSmoother[4];
  CParamSmooth mMixSmoother[4];
  
  CParamSmooth mDrive1Smoother = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mOutput1Smoother = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mDrive2Smoother = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mOutput2Smoother = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mDrive3Smoother = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mOutput3Smoother = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mDrive4Smoother = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mOutput4Smoother = CParamSmooth(5.0,GetSampleRate());
  
  CParamSmooth mCrossoverSmoother1 = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mCrossoverSmoother2 = CParamSmooth(5.0,GetSampleRate());
  CParamSmooth mCrossoverSmoother3 = CParamSmooth(5.0,GetSampleRate());
  
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

  IBitmapControl* mBand1LevelMeter;
  IBitmapControl* mBand2LevelMeter;
  IBitmapControl* mBand3LevelMeter;
  IBitmapControl* mBand4LevelMeter;

  
  PeakFollower* mPeakFollower;

  
  //Set Colors
  IColor DARK_GRAY = IColor(255,50,50,50);
  IColor LIGHT_GRAY = IColor(255,70,70,70);
  IColor MID_GRAY = IColor(255,60,60,60);
  IColor LIGHTER_GRAY = IColor(255,128,128,128);
  IColor LIGHT_ORANGE = IColor(255,245,187,0);
  IColor DARK_ORANGE = IColor(255,236,159,5);
  IColor TRANSP_ORANGE = IColor(255,245*.22,187*.22,0);
  
  
  LinkwitzRiley band1lp;
  LinkwitzRiley band2hp;
  LinkwitzRiley band2lp;
  LinkwitzRiley band3hp;
  LinkwitzRiley band3lp;
  LinkwitzRiley band4hp;
  
  CFxRbjFilter allpass1;
  CFxRbjFilter allpass2;
  CFxRbjFilter allpass3;

  WDL_BesselFilterCoeffs mAntiAlias;
  WDL_BesselFilterStage mUpsample, mDownsample;
  
  double samplesFilteredDry[4];
  double samplesFilteredWet[4];
  double chebyshev[8];
  double mInputGain;
  double mOutputGain;
  double RMSDry, RMSWet;

  
  double mCrossoverFreq1;
  double mCrossoverFreq2;
  double mCrossoverFreq3;
  
  const int fftSize=4096;
  const int channelCount = 2;
  
  const int mOversampling;

  double mDrive[4];
  double mMix[4];
  int mDistMode[4];
  bool mMute[4];
  bool mSolo[4];
  bool mEnable[4];
  
  bool mDistModesLinked;
  bool mOutputClipping;
  bool mSpectBypass;

};

#endif
