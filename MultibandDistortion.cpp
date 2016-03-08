#include "MultibandDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kDistType = 0,
  kNumPolynomials,
  kInputGain,
  kDrive1,
  kDrive2,
  kDrive3,
  kDrive4,
  kMix1,
  kMix2,
  kMix3,
  kMix4,
  kOutputGain,
  kAutoGainComp,
  kBand1Bypass,
  kBand2Bypass,
  kBand3Bypass,
  kBand4Bypass,
  kOutputClipping,
  kDistMode1,
  kDistMode2,
  kDistMode3,
  kDistMode4,
  kSolo1,
  kSolo2,
  kSolo3,
  kSolo4,
  kMute1,
  kMute2,
  kMute3,
  kMute4,
  kSpectBypass,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  kNumModes,
  
  kDriveY = 135,
  kDrive1X = 50,
  kDrive2X = kDrive1X+100,
  kDrive3X = kDrive2X+100,
  kDrive4X = kDrive3X+100,
  
  kMix1X = 85,
  kMix2X = kMix1X+100,
  kMix3X = kMix2X+100,
  kMix4X = kMix3X+100,
  
  kSpectBypassX = 42,
  kSpectBypassY = 22,
  
  kSliderFrames=33
};

MultibandDistortion::MultibandDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mInputGain(0.), mOutputGain(0.)
{
  TRACE;
  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&LIGHT_GRAY);
  
  //Initialize Parameters
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDistType)->InitInt("Distortion Type", 1, 1, 8);
  GetParam(kNumPolynomials)->InitInt("Num Chebyshev Polynomials", 3, 1, 5);
  GetParam(kInputGain)->InitDouble("Input Gain", 0., -36., 36., 0.0001, "dB");
  GetParam(kOutputGain)->InitDouble("Output Gain", 0., -36., 36., 0.0001, "dB");
  GetParam(kAutoGainComp)->InitBool("Auto Gain Compensation", true);
  GetParam(kOutputClipping)->InitBool("Output Clipping", false);
  GetParam(kSpectBypass)->InitBool("Analyzer On", true);
  
  
  GetParam(kDrive1)->InitDouble("Band 1: Drive", 0., 0., 36., 0.0001, "dB");
  GetParam(kDrive2)->InitDouble("Band 2: Drive", 0., 0., 36., 0.0001, "dB");
  GetParam(kDrive3)->InitDouble("Band 3: Drive", 0., 0., 36., 0.0001, "dB");
  GetParam(kDrive4)->InitDouble("Band 4: Drive", 0., 0., 36., 0.0001, "dB");

  GetParam(kMix1)->InitDouble("Band 1: Mix", 100., 0., 100., 0.001, "%");
  GetParam(kMix2)->InitDouble("Band 2: Mix", 100., 0., 100., 0.001, "%");
  GetParam(kMix3)->InitDouble("Band 3: Mix", 100., 0., 100., 0.001, "%");
  GetParam(kMix4)->InitDouble("Band 4: Mix", 100., 0., 100., 0.001, "%");

  GetParam(kBand1Bypass)->InitBool("Band 1: Bypass", true);
  GetParam(kBand2Bypass)->InitBool("Band 2: Bypass", true);
  GetParam(kBand3Bypass)->InitBool("Band 3: Bypass", true);
  GetParam(kBand4Bypass)->InitBool("Band 4: Bypass", true);

  GetParam(kSolo1)->InitBool("Band 1: Solo", false);
  GetParam(kSolo2)->InitBool("Band 2: Solo", false);
  GetParam(kSolo3)->InitBool("Band 3: Solo", false);
  GetParam(kSolo4)->InitBool("Band 4: Solo", false);
  
  GetParam(kMute1)->InitBool("Band 1: Mute", false);
  GetParam(kMute2)->InitBool("Band 2: Mute", false);
  GetParam(kMute3)->InitBool("Band 3: Mute", false);
  GetParam(kMute4)->InitBool("Band 4: Mute", false);

  
  GetParam(kDistMode1)->InitEnum("Band 1: Mode", 0, kNumModes);
  GetParam(kDistMode1)->SetDisplayText(0, "Soft");
  GetParam(kDistMode1)->SetDisplayText(1, "Fat");
  GetParam(kDistMode1)->SetDisplayText(2, "Sine");
  GetParam(kDistMode1)->SetDisplayText(3, "Fold");
  GetParam(kDistMode1)->SetDisplayText(4, "Cheby");
  GetParam(kDistMode1)->SetDisplayText(5, "Tube");

  GetParam(kDistMode2)->InitEnum("Band 2: Mode", 0, kNumModes);
  GetParam(kDistMode2)->SetDisplayText(0, "Soft");
  GetParam(kDistMode2)->SetDisplayText(1, "Fat");
  GetParam(kDistMode2)->SetDisplayText(2, "Sine");
  GetParam(kDistMode2)->SetDisplayText(3, "Fold");
  GetParam(kDistMode2)->SetDisplayText(4, "Cheby");
  GetParam(kDistMode2)->SetDisplayText(5, "Tube");

  GetParam(kDistMode3)->InitEnum("Band 3: Mode", 0, kNumModes);
  GetParam(kDistMode3)->SetDisplayText(0, "Soft");
  GetParam(kDistMode3)->SetDisplayText(1, "Fat");
  GetParam(kDistMode3)->SetDisplayText(2, "Sine");
  GetParam(kDistMode3)->SetDisplayText(3, "Fold");
  GetParam(kDistMode3)->SetDisplayText(4, "Cheby");
  GetParam(kDistMode3)->SetDisplayText(5, "Tube");

  GetParam(kDistMode4)->InitEnum("Band 4: Mode", 0, kNumModes);
  GetParam(kDistMode4)->SetDisplayText(0, "Soft");
  GetParam(kDistMode4)->SetDisplayText(1, "Fat");
  GetParam(kDistMode4)->SetDisplayText(2, "Sine");
  GetParam(kDistMode4)->SetDisplayText(3, "Fold");
  GetParam(kDistMode4)->SetDisplayText(4, "Cheby");
  GetParam(kDistMode4)->SetDisplayText(5, "Tube");

  //Knobs/sliders
  IBitmap slider = pGraphics->LoadIBitmap(SLIDER_ID, SLIDER_FN, kSliderFrames);
  IBitmap bypass = pGraphics->LoadIBitmap(BYPASS_ID, BYPASS_FN, 2);
  IBitmap bypassSmall = pGraphics->LoadIBitmap(BYPASSSMALL_ID, BYPASSSMALL_FN, 2);
  IBitmap solo = pGraphics->LoadIBitmap(SOLO_ID, SOLO_FN,2);
  IBitmap mute = pGraphics->LoadIBitmap(MUTE_ID, MUTE_FN,2);
  
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive1X, kDriveY, kDrive1, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive2X, kDriveY, kDrive2, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive3X, kDriveY, kDrive3, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive4X, kDriveY, kDrive4, &slider));

  pGraphics->AttachControl(new IKnobMultiControl(this, kMix1X, kDriveY, kMix1, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMix2X, kDriveY, kMix2, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMix3X, kDriveY, kMix3, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMix4X, kDriveY, kMix4, &slider));

  pGraphics->AttachControl(new ISwitchControl(this, kDrive1X, kDriveY+180, kBand1Bypass, &bypass));
  pGraphics->AttachControl(new ISwitchControl(this, kDrive2X, kDriveY+180, kBand2Bypass, &bypass));
  pGraphics->AttachControl(new ISwitchControl(this, kDrive3X, kDriveY+180, kBand3Bypass, &bypass));
  pGraphics->AttachControl(new ISwitchControl(this, kDrive4X, kDriveY+180, kBand4Bypass, &bypass));
  
  
  mSoloControl1 = new ISwitchControl(this, kDrive1X+44, kDriveY+180, kSolo1, &solo);
  mSoloControl2 = new ISwitchControl(this, kDrive2X+44, kDriveY+180, kSolo2, &solo);
  mSoloControl3 = new ISwitchControl(this, kDrive3X+44, kDriveY+180, kSolo3, &solo);
  mSoloControl4 = new ISwitchControl(this, kDrive4X+44, kDriveY+180, kSolo4, &solo);
  pGraphics->AttachControl(mSoloControl1);
  pGraphics->AttachControl(mSoloControl2);
  pGraphics->AttachControl(mSoloControl3);
  pGraphics->AttachControl(mSoloControl4);

  mMuteControl1 = new ISwitchControl(this, kDrive1X+44, kDriveY+196, kMute1, &mute);
  mMuteControl2 = new ISwitchControl(this, kDrive2X+44, kDriveY+196, kMute2, &mute);
  mMuteControl3 = new ISwitchControl(this, kDrive3X+44, kDriveY+196, kMute3, &mute);
  mMuteControl4 = new ISwitchControl(this, kDrive4X+44, kDriveY+196, kMute4, &mute);
  
  pGraphics->AttachControl(mMuteControl1);
  pGraphics->AttachControl(mMuteControl2);
  pGraphics->AttachControl(mMuteControl3);
  pGraphics->AttachControl(mMuteControl4);
  
  IText text = IText(14, &COLOR_WHITE, "Futura");
  
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive1X, kDriveY+130, kDrive1X+24, kDriveY+140), &text, "DRV"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix1X, kDriveY+130, kMix1X+24, kDriveY+140), &text, "MIX"));
 
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive2X, kDriveY+130, kDrive2X+24, kDriveY+140), &text, "DRV"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix2X, kDriveY+130, kMix2X+24, kDriveY+140), &text, "MIX"));
 
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive3X, kDriveY+130, kDrive3X+24, kDriveY+140), &text, "DRV"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix3X, kDriveY+130, kMix3X+24, kDriveY+140), &text, "MIX"));
  
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive4X, kDriveY+130, kDrive4X+24, kDriveY+140), &text, "DRV"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix4X, kDriveY+130, kMix4X+24, kDriveY+140), &text, "MIX"));
  
  IRECT modeRect1 = IRECT(kDrive1X, kDriveY+154, kDrive1X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect1, DARK_GRAY, LIGHT_GRAY, kDistMode1));
  
  IRECT modeRect2 = IRECT(kDrive2X, kDriveY+154, kDrive2X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect2, DARK_GRAY, LIGHT_GRAY, kDistMode2));

  IRECT modeRect3 = IRECT(kDrive3X, kDriveY+154, kDrive3X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect3, DARK_GRAY, LIGHT_GRAY, kDistMode3));
  
  IRECT modeRect4 = IRECT(kDrive4X, kDriveY+154, kDrive4X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect4, DARK_GRAY, LIGHT_GRAY, kDistMode4));
  
  //Vertical lines
  pGraphics->DrawVerticalLine(&DARK_GRAY, kDrive2X-20, kDriveY, kDriveY+40);
  
  
  //Initialize Parameter Smoothers
  mInputGainSmoother = new CParamSmooth(5.0, GetSampleRate());
  mOutputGainSmoother = new CParamSmooth(5.0, GetSampleRate());
  mDrive1Smoother = new CParamSmooth(5.0, GetSampleRate());
  mOutput1Smoother = new CParamSmooth(5.0, GetSampleRate());
  mDrive2Smoother = new CParamSmooth(5.0, GetSampleRate());
  mOutput2Smoother = new CParamSmooth(5.0, GetSampleRate());
  mDrive3Smoother = new CParamSmooth(5.0, GetSampleRate());
  mOutput3Smoother = new CParamSmooth(5.0, GetSampleRate());
  mDrive4Smoother = new CParamSmooth(5.0, GetSampleRate());
  mOutput4Smoother = new CParamSmooth(5.0, GetSampleRate());
  
  //Initialize RMS followers
  mRMSBand1Dry= new RMSFollower();
  mRMSBand1Wet= new RMSFollower();
  mRMSBand2Dry= new RMSFollower();
  mRMSBand2Wet= new RMSFollower();
  mRMSBand3Dry= new RMSFollower();
  mRMSBand3Wet= new RMSFollower();
  mRMSBand4Dry= new RMSFollower();
  mRMSBand4Wet= new RMSFollower();
  
  
  //IRECT For FFT Analyzer
  IRECT iView(40, 20, GUI_WIDTH-40, 20+100);
  
 
  gAnalyzer = new gFFTAnalyzer(this, iView, COLOR_WHITE, -1, fftSize, false);
  pGraphics->AttachControl((IControl*)gAnalyzer);
  gAnalyzer->SetdbFloor(-60.);
  gAnalyzer->SetColors(LIGHT_ORANGE, DARK_ORANGE, DARK_GRAY);

#ifdef OS_OSX
  char* fontName = "Futura";
  IText::EQuality texttype = IText::kQualityAntiAliased;
#else
  char* fontName = "Calibri";
  IText::EQuality texttype = IText::EQuality::kQualityClearType;
  
#endif
  
  IText lFont(12, &COLOR_WHITE, fontName, IText::kStyleNormal, IText::kAlignCenter, 0, texttype);
  // adding the vertical frequency lines
  gFreqLines = new gFFTFreqDraw(this, iView, LIGHT_GRAY, &lFont);

  
  pGraphics->AttachControl((IControl*)gFreqLines);
  
  pGraphics->AttachControl(new ISwitchControl(this, kSpectBypassX, kSpectBypassY, kSpectBypass, &bypassSmall));

  //Vertical lines
  
  
  //setting the min/max freq for fft display and freq lines
  const double maxF = 20000.;
  const double minF = 20.;
  gAnalyzer->SetMaxFreq(maxF);
  gFreqLines->SetMaxFreq(maxF);
  gAnalyzer->SetMinFreq(minF);
  gFreqLines->SetMinFreq(minF);
  //setting +3dB/octave compensation to the fft display
  gAnalyzer->SetOctaveGain(3., true);

  AttachGraphics(pGraphics);

  
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  
  //initializing FFT class
  sFFT = new Spect_FFT(this, fftSize, 2);
  sFFT->SetWindowType(Spect_FFT::win_BlackmanHarris);

}

MultibandDistortion::~MultibandDistortion() {}


double MultibandDistortion::ProcessDistortion(double sample, int distType)
{
  //Soft asymmetrical clipping
  //algorithm by Bram de Jong, from musicdsp.org archives
  if (distType==0) {
    double threshold = 0.9;
    
    if(sample>threshold)
      sample = threshold + (sample - threshold) / (1 + pow(((sample - threshold)/(1 - threshold)), 2));
    else if(sample >1)
      sample = (sample + 1)/2;
  }
  
  //arctan waveshaper
  else if(distType==1){
    double amount = 3;
    sample =  fastAtan(sample * amount);
  }
  
  //Sine shaper
  //based on Jon Watte's waveshaper algorithm. Modified for softer clipping
  else if(distType==2){
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
  else if(distType==3){
    double threshold = .9;
    if (sample > threshold || sample < - threshold) {
      sample = fabs(fabs(fmod(sample - threshold, threshold * 4)) - threshold * 2) - threshold;
    }
  }
  
  //First 5 order chebyshev polynomials
  else if (distType==4){
    //sample = 4*pow(sample,3)-3*sample + 2*sample*sample  + sample;
    
    double chebyshev[7];
    chebyshev[0] = sample;
    chebyshev[1] = 2 * sample * sample - 1;
    chebyshev[2] = 4 * pow(sample, 3) - 3 * sample;
    chebyshev[3] = 8 * pow(sample, 4) - 8 * sample * sample + 1;
    chebyshev[4] = 16 * pow(sample, 5) - 20 * pow(sample,3) - 7 * sample;
    for(int i=1; i<mPolynomials; i++){
      sample+=chebyshev[i];
      sample*=.5;
    }
  }
  
  //tube emulation
  else if (distType==5){
    sample = sin(sample) + pow(abs(sin(sample)), 2) - .1;
    sample = sin(sample) + pow(abs(sin(sample)), 4) - .1;
    sample = sin(sample) + pow(abs(sin(sample)), 8) - .1;

  }
  return sample;
}

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
      double sample, sampleBand1, sampleBand2, sampleBand3, sampleBand4, rms1Dry, rms1Wet, rms2Dry, rms2Wet,rms3Dry, rms3Wet,rms4Dry, rms4Wet;
       sample = *input;
      double samplesFiltered[4];
      //Apply input gain
      sample *= DBToAmp(mInputGainSmoother->process(mInputGain)); //parameter smoothing prevents artifacts when changing parameter value
      
      sample = ProcessDistortion(sample, 2);

      
      samplesFiltered[0] = sample;
      samplesFiltered[1] = sample;
      samplesFiltered[2] = sample;
      samplesFiltered[3] = sample;
      
      if (mAutoGainComp) {
        rms1Dry =  mRMSBand1Dry->getRMS(sampleBand1);
        rms2Dry =  mRMSBand1Dry->getRMS(sampleBand2);
        rms3Dry =  mRMSBand1Dry->getRMS(sampleBand3);
        rms4Dry =  mRMSBand1Dry->getRMS(sampleBand4);

      }
      
      if (mSolo1) {
        
      }
      
      
      if (mMute1) {
        sampleBand1=0;
      }
      else{
        sampleBand1*=DBToAmp(mDrive1Smoother->process(sampleBand1));
        sampleBand1=ProcessDistortion(sampleBand1, mDistMode1);
      }
      
      if (mMute2) {
        sampleBand2=0;
      }
      else{
        sampleBand2*= DBToAmp(mDrive1Smoother->process(sampleBand2));
        sampleBand2=ProcessDistortion(sampleBand2, mDistMode2);
      }
      
      if (mMute3) {
        sampleBand3=0;
      }
      else{
        sampleBand3*=DBToAmp(mDrive1Smoother->process(sampleBand3));
        sampleBand3=ProcessDistortion(sampleBand3, mDistMode3);
      }
      
      if (mMute4) {
        sampleBand4=0;
      }
      else{
        sampleBand4*=DBToAmp(mDrive1Smoother->process(sampleBand4));
        sampleBand4=ProcessDistortion(sampleBand4, mDistMode4);
      }
      
      if (mAutoGainComp) {
        rms1Wet =  mRMSBand1Dry->getRMS(sampleBand1);
        rms2Wet =  mRMSBand1Dry->getRMS(sampleBand2);
        rms3Wet =  mRMSBand1Dry->getRMS(sampleBand3);
        rms4Wet =  mRMSBand1Dry->getRMS(sampleBand4);
        
        //Gain Compensation
        sampleBand1*=rms1Dry/rms1Wet;
        sampleBand2*=rms2Dry/rms2Wet;
        sampleBand3*=rms3Dry/rms3Wet;
        sampleBand4*=rms4Dry/rms4Wet;
      }

      sample = (sampleBand1+sampleBand2+sampleBand3+sampleBand4)/4.;

      
      //Clipping
      if(mOutputClipping){
        if (sample>1) {
          sample = DBToAmp(-0.1);
        }
        else if (sample<-1) {
          sample = -1*DBToAmp(-0.1);
        }
      }
      
      if(mSpectBypass) sFFT->SendInput(sample);

      
      *output = sample;
    }
  }
  
  if (GetGUI() && mSpectBypass) {
    const double sr = this->GetSampleRate();
    for (int c = 0; c < fftSize / 2 + 1; c++) {
      gAnalyzer->SendFFT(sFFT->GetOutput(c), c, sr);
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
      
    case kNumPolynomials:
      mPolynomials = GetParam(kNumPolynomials)->Value();
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
      
    case kDrive1:
      mDrive1=GetParam(kDrive1)->Value();
      break;

    case kDrive2:
      mDrive2=GetParam(kDrive2)->Value();
      break;
      
    case kDrive3:
      mDrive3=GetParam(kDrive3)->Value();
      break;
      
    case kDrive4:
      mDrive4=GetParam(kDrive4)->Value();
      break;
      
    case kMix1:
      mMix1=GetParam(kMix1)->Value();
      break;

    case kMix2:
      mMix2=GetParam(kMix2)->Value();
      break;
      
    case kMix3:
      mMix3=GetParam(kMix3)->Value();
      break;
      
    case kMix4:
      mMix4=GetParam(kMix4)->Value();
      break;
      
    case kBand1Bypass:
      mBand1Bypass=GetParam(kBand1Bypass)->Value();
      break;
      
    case kBand2Bypass:
      mBand2Bypass=GetParam(kBand2Bypass)->Value();
      break;
      
    case kBand3Bypass:
      mBand3Bypass=GetParam(kBand3Bypass)->Value();
      break;
     
    case kBand4Bypass:
      mBand4Bypass=GetParam(kBand4Bypass)->Value();
      break;
      
    case kDistMode1:
      mDistMode1=GetParam(kDistMode1)->Value();
      break;
      
    case kDistMode2:
      mDistMode2=GetParam(kDistMode2)->Value();
      break;
    
    case kDistMode3:
      mDistMode3=GetParam(kDistMode3)->Value();
      break;
      
    case kDistMode4:
      mDistMode4=GetParam(kDistMode4)->Value();
      break;
      
    case kSpectBypass:
      mSpectBypass=GetParam(kSpectBypass)->Value();
      break;
      
    case kSolo1:
      mSolo1=GetParam(kSolo1)->Value();
      if(mSolo1){
        mSoloControl2->SetValueFromPlug(false);
        mSoloControl3->SetValueFromPlug(false);
        mSoloControl4->SetValueFromPlug(false);
        
        mMuteControl1->SetValueFromPlug(false);
      }
      break;
      
    case kSolo2:
      mSolo2=GetParam(kSolo2)->Value();
      if(mSolo2){
        mSoloControl1->SetValueFromPlug(false);
        mSoloControl3->SetValueFromPlug(false);
        mSoloControl4->SetValueFromPlug(false);
        
        mMuteControl2->SetValueFromPlug(false);
      }
      break;
      
    case kSolo3:
      mSolo3=GetParam(kSolo3)->Value();
      if(mSolo3){
        mSoloControl1->SetValueFromPlug(false);
        mSoloControl2->SetValueFromPlug(false);
        mSoloControl4->SetValueFromPlug(false);
        
        mMuteControl3->SetValueFromPlug(false);
      }
      break;
      
    case kSolo4:
      mSolo4=GetParam(kSolo4)->Value();
      if(mSolo4){
        mSoloControl1->SetValueFromPlug(false);
        mSoloControl2->SetValueFromPlug(false);
        mSoloControl3->SetValueFromPlug(false);
        
        mMuteControl4->SetValueFromPlug(false);
      }
      break;
      
    case kMute1:
      mMute1=GetParam(kMute1)->Value();
      break;
      
    case kMute2:
      mMute2=GetParam(kMute2)->Value();
      break;
      
    case kMute3:
      mMute3=GetParam(kMute3)->Value();
      break;
      
    case kMute4:
      mMute4=GetParam(kMute4)->Value();
      break;
      
    default:
      break;
  }
}

double MultibandDistortion::fastAtan(double x){
  return (x / (1.0 + 0.28 * (x * x)));
}