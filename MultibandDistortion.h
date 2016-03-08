#ifndef __MULTIBANDDISTORTION__
#define __MULTIBANDDISTORTION__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
#include "FFTRect.h"
#include "IPopupMenuControl.h"
#include "RMS.h"

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
  
  CParamSmooth* mDrive1Smoother;
  CParamSmooth* mOutput1Smoother;
  CParamSmooth* mDrive2Smoother;
  CParamSmooth* mOutput2Smoother;
  CParamSmooth* mDrive3Smoother;
  CParamSmooth* mOutput3Smoother;
  CParamSmooth* mDrive4Smoother;
  CParamSmooth* mOutput4Smoother;
  
  ISwitchControl* mSoloControl1;
  ISwitchControl* mSoloControl2;
  ISwitchControl* mSoloControl3;
  ISwitchControl* mSoloControl4;

  ISwitchControl* mMuteControl1;
  ISwitchControl* mMuteControl2;
  ISwitchControl* mMuteControl3;
  ISwitchControl* mMuteControl4;
  
  //Set Colors
  IColor DARK_GRAY = IColor(255,50,50,50);
  IColor LIGHT_GRAY = IColor(255,70,70,70);
  IColor LIGHT_ORANGE = IColor(255,245,187,0);
  IColor DARK_ORANGE = IColor(255,236,159,5);
  
  RMSFollower mRMSDry[4];
  RMSFollower mRMSWet[4];

  
  RMSFollower* mRMSBand1Dry;
  RMSFollower* mRMSBand1Wet;
  RMSFollower* mRMSBand2Dry;
  RMSFollower* mRMSBand2Wet;
  RMSFollower* mRMSBand3Dry;
  RMSFollower* mRMSBand3Wet;
  RMSFollower* mRMSBand4Dry;
  RMSFollower* mRMSBand4Wet;
  
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
  
  double mDrive[4];
  double mMixe[4];
  int mDistMode[4];
  bool mMute[4];
  bool mSolo[4];
  bool mBypass[4];
  
  bool mAutoGainComp;
  bool mOutputClipping;
  
  bool mSpectBypass;
  
  bool mBand1Bypass;
  bool mBand2Bypass;
  bool mBand3Bypass;
  bool mBand4Bypass;
  
  
  bool mSolo1;
  bool mSolo2;
  bool mSolo3;
  bool mSolo4;

  bool mMute1;
  bool mMute2;
  bool mMute3;
  bool mMute4;

};

#endif
