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
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  kDriveY = 135,
  kDrive1X = 50,
  kDrive2X = kDrive1X+100,
  kDrive3X = kDrive2X+100,
  kDrive4X = kDrive3X+100,
  
  kMix1X = 85,
  kMix2X = kMix1X+100,
  kMix3X = kMix2X+100,
  kMix4X = kMix3X+100,
  
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
  
  GetParam(kBand1Bypass)->InitBool("Band 1: Bypass", true);
  GetParam(kBand2Bypass)->InitBool("Band 2: Bypass", true);
  GetParam(kBand3Bypass)->InitBool("Band 3: Bypass", true);
  GetParam(kBand4Bypass)->InitBool("Band 4: Bypass", true);



  GetParam(kDistMode1)->InitEnum("Mode", 0, 5);
  GetParam(kDistMode1)->SetDisplayText(0, "Soft");
  GetParam(kDistMode1)->SetDisplayText(1, "Fat");
  GetParam(kDistMode1)->SetDisplayText(2, "Sine");
  GetParam(kDistMode1)->SetDisplayText(3, "Fold");
  GetParam(kDistMode1)->SetDisplayText(4, "Tube");

  GetParam(kDistMode2)->InitEnum("Mode", 0, 5);
  GetParam(kDistMode2)->SetDisplayText(0, "Soft");
  GetParam(kDistMode2)->SetDisplayText(1, "Fat");
  GetParam(kDistMode2)->SetDisplayText(2, "Sine");
  GetParam(kDistMode2)->SetDisplayText(3, "Fold");
  GetParam(kDistMode2)->SetDisplayText(4, "Tube");
  
  GetParam(kDistMode3)->InitEnum("Mode", 0, 5);
  GetParam(kDistMode3)->SetDisplayText(0, "Soft");
  GetParam(kDistMode3)->SetDisplayText(1, "Fat");
  GetParam(kDistMode3)->SetDisplayText(2, "Sine");
  GetParam(kDistMode3)->SetDisplayText(3, "Fold");
  GetParam(kDistMode3)->SetDisplayText(4, "Tube");
  
  GetParam(kDistMode4)->InitEnum("Mode", 0, 5);
  GetParam(kDistMode4)->SetDisplayText(0, "Soft");
  GetParam(kDistMode4)->SetDisplayText(1, "Fat");
  GetParam(kDistMode4)->SetDisplayText(2, "Sine");
  GetParam(kDistMode4)->SetDisplayText(3, "Fold");
  GetParam(kDistMode4)->SetDisplayText(4, "Tube");
  
  //Knobs/sliders
  IBitmap slider = pGraphics->LoadIBitmap(SLIDER_ID, SLIDER_FN, kSliderFrames);
  IBitmap bypass = pGraphics->LoadIBitmap(BYPASS_ID, BYPASS_FN, 2);

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
  
  
  IText text = IText(14, &COLOR_WHITE, "Futura");
  
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive1X, kDriveY+130, kDrive1X+24, kDriveY+140), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix1X, kDriveY+130, kMix1X+24, kDriveY+140), &text, "MIX"));
 
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive2X, kDriveY+130, kDrive2X+24, kDriveY+140), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix2X, kDriveY+130, kMix2X+24, kDriveY+140), &text, "MIX"));
 
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive3X, kDriveY+130, kDrive3X+24, kDriveY+140), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix3X, kDriveY+130, kMix3X+24, kDriveY+140), &text, "MIX"));
  
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive4X, kDriveY+130, kDrive4X+24, kDriveY+140), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix4X, kDriveY+130, kMix4X+24, kDriveY+140), &text, "MIX"));
  
  IRECT modeRect1 = IRECT(kDrive1X, kDriveY+154, kDrive1X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect1, DARK_GRAY, LIGHT_GRAY, kDistMode1));
  
  IRECT modeRect2 = IRECT(kDrive2X, kDriveY+154, kDrive2X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect2, DARK_GRAY, LIGHT_GRAY, kDistMode2));

  IRECT modeRect3 = IRECT(kDrive3X, kDriveY+154, kDrive3X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect3, DARK_GRAY, LIGHT_GRAY, kDistMode3));
  
  IRECT modeRect4 = IRECT(kDrive4X, kDriveY+154, kDrive4X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect4, DARK_GRAY, LIGHT_GRAY, kDistMode4));

  //Initialize Parameter Smoothers
  mInputGainSmoother = new CParamSmooth(5.0, GetSampleRate());
  mOutputGainSmoother = new CParamSmooth(5.0, GetSampleRate());

  

  
  
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
  if (distType==1) {
    double threshold = 0.9;
    
    if(sample>threshold)
      sample = threshold + (sample - threshold) / (1 + pow(((sample - threshold)/(1 - threshold)), 2));
    else if(sample >1)
      sample = (sample + 1)/2;
  }
  
  //arctan waveshaper
  else if(distType==2){
    double amount = 3;
    sample =  fastAtan(sample * amount);
  }
  
  //Sine shaper
  //based on Jon Watte's waveshaper algorithm. Modified for softer clipping
  else if(distType==3){
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
  else if(distType==4){
    double threshold = .9;
    if (sample > threshold || sample < - threshold) {
      sample = fabs(fabs(fmod(sample - threshold, threshold * 4)) - threshold * 2) - threshold;
    }
  }
  
  //First 5 order chebyshev polynomials
  else if (distType==5){
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
      double sample = *input;
      
      //Apply input gain
      sample *= DBToAmp(mInputGainSmoother->process(mInputGain)); //parameter smoothing prevents artifacts when changing parameter value
      
      //sample = ProcessDistortion(sample, mDistType);

      //Apply output gainnd
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
      
      
      sFFT->SendInput(sample);
      
      *output = sample;
    }
  }
  
  if (GetGUI()) {
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
      
    default:
      break;
  }
}

double MultibandDistortion::fastAtan(double x){
  return (x / (1.0 + 0.28 * (x * x)));
}