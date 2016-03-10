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
  kBand1Enable,
  kBand2Enable,
  kBand3Enable,
  kBand4Enable,
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
  kDistModeLinked,
  kSpectBypass,
  kCrossoverFreq1,
  kCrossoverFreq2,
  kCrossoverFreq3,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  kNumModes=6,
  
  kDriveY = 132,
  kDrive1X = 45,
  kDrive2X = kDrive1X+104,
  kDrive3X = kDrive2X+104,
  kDrive4X = kDrive3X+104,
  
  kMix1X = kDrive1X+35,
  kMix2X = kMix1X+104,
  kMix3X = kMix2X+104,
  kMix4X = kMix3X+104,
  
  kSpectBypassX = 27,
  kSpectBypassY = 22,
  
  kSliderFrames=33
};

MultibandDistortion::MultibandDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mInputGain(0.), mOutputGain(0.), mDistModesLinked(false), mAutoGainComp(false)
{
  TRACE;

//  band1lp = new LinkwitzRiley(GetSampleRate(), Lowpass, 112);
//  band2hp = new LinkwitzRiley(GetSampleRate(), Highpass, 112);
//  band2lp = new LinkwitzRiley(GetSampleRate(), Lowpass, 637);
//  band3hp = new LinkwitzRiley(GetSampleRate(), Highpass, 637);
//  band3lp = new LinkwitzRiley(GetSampleRate(), Lowpass, 3600);
//  band4hp = new LinkwitzRiley(GetSampleRate(), Highpass, 3600);

  band1lp = new CFilterButterworth24db();
  band1lp->SetSampleRate(GetSampleRate());
  band1lp->set
  band2hp = new CFilterButterworth24db();
  band2lp = new CFilterButterworth24db();
  band3hp = new CFilterButterworth24db();
  band3lp = new CFilterButterworth24db();
  band4hp = new CFilterButterworth24db();

  
  //Initialize Parameter Smoothers + RMS Level Followers
  mInputGainSmoother=new CParamSmooth(5.0,GetSampleRate());
  mDriveSmoother= new CParamSmooth*[4];
  mOutputSmoother= new CParamSmooth*[4];
  mMixSmoother= new CParamSmooth*[4];
  mRMSDry= new RMSFollower*[4];
  mRMSWet= new RMSFollower*[4];
  
  for (int i=0; i<4; i++) {
    mDriveSmoother[i]=new CParamSmooth(5.0,GetSampleRate());
    mOutputSmoother[i]=new CParamSmooth(5.0,GetSampleRate());
    mMixSmoother[i]=new CParamSmooth(5.0,GetSampleRate());
    mRMSDry[i]=new RMSFollower();
    mRMSWet[i]=new RMSFollower;
  }

  //======================================================================================================
  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&LIGHT_GRAY);
  

  //Initialize Parameters
  //
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kCrossoverFreq1)->InitDouble("Crossover 1: Freq", 112., 20., 20000., .0001, "Hz");
  GetParam(kCrossoverFreq2)->InitDouble("Crossover 2: Freq", 637., 20., 20000., .0001, "Hz");
  GetParam(kCrossoverFreq3)->InitDouble("Crossover 3: Freq", 3600., 20., 20000., .0001, "Hz");

  
  GetParam(kDistType)->InitInt("Distortion Type", 1, 1, 8);
  GetParam(kNumPolynomials)->InitInt("Num Chebyshev Polynomials", 3, 1, 5);
  GetParam(kInputGain)->InitDouble("Input Gain", 0., -36., 36., 0.0001, "dB");
  GetParam(kOutputGain)->InitDouble("Output Gain", 0., -36., 36., 0.0001, "dB");
  GetParam(kAutoGainComp)->InitBool("Auto Gain Compensation", false);
  GetParam(kOutputClipping)->InitBool("Output Clipping", false);
  GetParam(kSpectBypass)->InitBool("Analyzer On", true);
  GetParam(kDistModeLinked)->InitBool("Link Distortion Modes", false);
  
  GetParam(kDrive1)->InitDouble("Band 1: Drive", 0., 0., 10., 0.0001, "dB");
  GetParam(kDrive2)->InitDouble("Band 2: Drive", 0., 0., 10., 0.0001, "dB");
  GetParam(kDrive3)->InitDouble("Band 3: Drive", 0., 0., 10., 0.0001, "dB");
  GetParam(kDrive4)->InitDouble("Band 4: Drive", 0., 0., 10., 0.0001, "dB");

  GetParam(kMix1)->InitDouble("Band 1: Mix", 100., 0., 100., 0.001, "%");
  GetParam(kMix2)->InitDouble("Band 2: Mix", 100., 0., 100., 0.001, "%");
  GetParam(kMix3)->InitDouble("Band 3: Mix", 100., 0., 100., 0.001, "%");
  GetParam(kMix4)->InitDouble("Band 4: Mix", 100., 0., 100., 0.001, "%");
  GetParam(kMix1)->SetShape(2.);
  GetParam(kMix2)->SetShape(2.);
  GetParam(kMix3)->SetShape(2.);
  GetParam(kMix4)->SetShape(2.);


  GetParam(kBand1Enable)->InitBool("Band 1: Enable", true);
  GetParam(kBand2Enable)->InitBool("Band 2: Enable", true);
  GetParam(kBand3Enable)->InitBool("Band 3: Enable", true);
  GetParam(kBand4Enable)->InitBool("Band 4: Enable", true);

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

  //Bitmaps
  IBitmap slider = pGraphics->LoadIBitmap(SLIDER_ID, SLIDER_FN, kSliderFrames);
  IBitmap bypass = pGraphics->LoadIBitmap(BYPASS_ID, BYPASS_FN, 2);
  IBitmap bypassSmall = pGraphics->LoadIBitmap(BYPASSSMALL_ID, BYPASSSMALL_FN, 2);
  IBitmap solo = pGraphics->LoadIBitmap(SOLO_ID, SOLO_FN,2);
  IBitmap mute = pGraphics->LoadIBitmap(MUTE_ID, MUTE_FN,2);
  IBitmap link = pGraphics->LoadIBitmap(LINK_ID, LINK_FN,2);
  
  
  //Mode Link control
  pGraphics->AttachControl(new ISwitchControl(this, kDrive2X-36, kDriveY+154, kDistModeLinked, &link));
  
  //Drive+Mix sliders
  
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive1X, kDriveY, kDrive1, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive2X, kDriveY, kDrive2, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive3X, kDriveY, kDrive3, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDrive4X, kDriveY, kDrive4, &slider));
  
  pGraphics->AttachControl(new IKnobMultiControl(this, kMix1X, kDriveY, kMix1, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMix2X, kDriveY, kMix2, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMix3X, kDriveY, kMix3, &slider));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMix4X, kDriveY, kMix4, &slider));

  //Bypass controls

  pGraphics->AttachControl(new ISwitchControl(this, kDrive1X, kDriveY+180, kBand1Enable, &bypass));
  pGraphics->AttachControl(new ISwitchControl(this, kDrive2X, kDriveY+180, kBand2Enable, &bypass));
  pGraphics->AttachControl(new ISwitchControl(this, kDrive3X, kDriveY+180, kBand3Enable, &bypass));
  pGraphics->AttachControl(new ISwitchControl(this, kDrive4X, kDriveY+180, kBand4Enable, &bypass));
  
  //Mute+solo controls
  
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
  
  
  //Slider Labels
  
  IText text = IText(14, &COLOR_WHITE, "Futura");
  
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive1X, kDriveY+132, kDrive1X+24, kDriveY+142), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix1X, kDriveY+132, kMix1X+24, kDriveY+142), &text, "MIX"));
 
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive2X, kDriveY+132, kDrive2X+24, kDriveY+142), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix2X, kDriveY+132, kMix2X+24, kDriveY+142), &text, "MIX"));
 
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive3X, kDriveY+132, kDrive3X+24, kDriveY+142), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix3X, kDriveY+132, kMix3X+24, kDriveY+142), &text, "MIX"));
  
  pGraphics->AttachControl(new ITextControl(this, IRECT(kDrive4X, kDriveY+132, kDrive4X+24, kDriveY+142), &text, "DRIVE"));
  pGraphics->AttachControl(new ITextControl(this, IRECT(kMix4X, kDriveY+132, kMix4X+24, kDriveY+142), &text, "MIX"));
  
  
  //Dist mode selector menus
  
  IRECT modeRect1 = IRECT(kDrive1X, kDriveY+154, kDrive1X+59, kDriveY+171);
  pGraphics->AttachControl(new IPopUpMenuControl(this, modeRect1, DARK_GRAY, LIGHT_GRAY, kDistMode1));
  
  IRECT modeRect2 = IRECT(kDrive2X, kDriveY+154, kDrive2X+59, kDriveY+171);
  mDistMode2 = new IPopUpMenuControl(this, modeRect2, DARK_GRAY, LIGHT_GRAY, kDistMode2);
  pGraphics->AttachControl(mDistMode2);

  IRECT modeRect3 = IRECT(kDrive3X, kDriveY+154, kDrive3X+59, kDriveY+171);
  mDistMode3 = new IPopUpMenuControl(this, modeRect3, DARK_GRAY, LIGHT_GRAY, kDistMode3);
  pGraphics->AttachControl(mDistMode3);
  
  IRECT modeRect4 = IRECT(kDrive4X, kDriveY+154, kDrive4X+59, kDriveY+171);
  mDistMode4 = new IPopUpMenuControl(this, modeRect4, DARK_GRAY, LIGHT_GRAY, kDistMode4);
  pGraphics->AttachControl(mDistMode4);
  



  //================================================================================================================================
  //FFT Analyzer
  
  IRECT iView(25, 20, GUI_WIDTH-25, 20+100);
  
  gAnalyzer = new gFFTAnalyzer(this, iView, COLOR_WHITE, -1, fftSize, false);
  pGraphics->AttachControl((IControl*)gAnalyzer);
  gAnalyzer->SetdbFloor(-60.);
  gAnalyzer->SetColors(MID_GRAY, DARK_ORANGE, DARK_GRAY);

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

  
  //pGraphics->AttachControl((IControl*)gFreqLines);
  

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

  
  //==================================================================================================================================
  
  //Initialize crossover control
  pGraphics->AttachControl(new ICrossoverControl(this, IRECT(iView.L,iView.T,iView.R, iView.B-20), &LIGHTER_GRAY, &DARK_GRAY, &LIGHT_ORANGE, kCrossoverFreq1, kCrossoverFreq2, kCrossoverFreq3));
  
  pGraphics->AttachControl(new ISwitchControl(this, kSpectBypassX, kSpectBypassY, kSpectBypass, &bypassSmall));

  
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
    sample =  fastAtan(sample * 3);
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
    sample = sin(sample) + pow(std::abs(sin(sample)), 2) - .1;
    sample = sin(sample) + pow(std::abs(sin(sample)), 4) - .1;
    sample = sin(sample) + pow(std::abs(sin(sample)), 8) - .1;

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
      sample *= DBToAmp(mInputGainSmoother->process(mInputGain)); //parameter smoothing prevents popping when changing parameter value
   
      
      samplesFilteredDry[0]=band1lp->process(sample);
      samplesFilteredDry[1]=band2hp->process(sample);
      samplesFilteredDry[1]=band2lp->process(samplesFilteredDry[1]);
      samplesFilteredDry[2]=band3hp->process(sample);
      samplesFilteredDry[2]=band3lp->process(samplesFilteredDry[2]);
      samplesFilteredDry[3]=band4hp->process(sample);

      //Loop through bands, process samples
      for (int j=0; j<4; j++) {
        samplesFilteredWet[j]=samplesFilteredDry[j];
        if (mMute[j]) {
          samplesFilteredWet[j]=0;
        }
        else {
          if (mEnable[j]) {
            samplesFilteredWet[j]*=DBToAmp(mDriveSmoother[j]->process(mDrive[j]));
            
            if(mDistModesLinked){
              samplesFilteredWet[j]=ProcessDistortion(samplesFilteredWet[j], mDistMode[0]);
            }
            else{
              samplesFilteredWet[j]=ProcessDistortion(samplesFilteredWet[j], mDistMode[j]);
            }
            
            //Mix
            samplesFilteredWet[j]= mMix[j]*samplesFilteredWet[j]+(1-mMix[j])*samplesFilteredDry[j];
            
            //Auto gain compensation
            if (mAutoGainComp) {
              RMSDry=mRMSDry[j]->getRMS(samplesFilteredDry[j]);
              RMSWet=mRMSWet[j]->getRMS(samplesFilteredWet[j]);
              samplesFilteredWet[j]*=RMSDry/RMSWet;
            }
          }
        }
      }
  
      
      //Sum output
      sample=0;
      for(int j=0; j<4; j++){
        if (mSolo[j]) {
          sample=samplesFilteredWet[j];
          break;
        }
        else{
          sample+=samplesFilteredWet[j];
        }
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
      mDrive[0]=GetParam(kDrive1)->Value();
      break;

    case kDrive2:
      mDrive[1]=GetParam(kDrive2)->Value();
      break;
      
    case kDrive3:
      mDrive[2]=GetParam(kDrive3)->Value();
      break;
      
    case kDrive4:
      mDrive[3]=GetParam(kDrive4)->Value();
      break;
      
    case kMix1:
      mMix[0]=GetParam(kMix1)->Value()/100.;
      break;

    case kMix2:
      mMix[1]=GetParam(kMix2)->Value()/100.;
      break;
      
    case kMix3:
      mMix[2]=GetParam(kMix3)->Value()/100.;
      break;
      
    case kMix4:
      mMix[3]=GetParam(kMix4)->Value()/ 100.;
      break;
      
    case kBand1Enable:
      mEnable[0]=GetParam(kBand1Enable)->Value();
      break;
      
    case kBand2Enable:
      mEnable[1]=GetParam(kBand2Enable)->Value();
      break;
      
    case kBand3Enable:
      mEnable[2]=GetParam(kBand3Enable)->Value();
      break;
     
    case kBand4Enable:
      mEnable[3]=GetParam(kBand4Enable)->Value();
      break;
      
    case kDistModeLinked:
      mDistModesLinked=GetParam(kDistModeLinked)->Value();
      if (mDistModesLinked) {
        mDistMode2->GrayOut(true);
        mDistMode3->GrayOut(true);
        mDistMode4->GrayOut(true);
      }
      else{
        mDistMode2->GrayOut(false);
        mDistMode3->GrayOut(false);
        mDistMode4->GrayOut(false);
      }
      break;
      
    case kDistMode1:
      mDistMode[0]=GetParam(kDistMode1)->Value();
      break;
      
    case kDistMode2:
      mDistMode[1]=GetParam(kDistMode2)->Value();
      break;
    
    case kDistMode3:
      mDistMode[2]=GetParam(kDistMode3)->Value();
      break;
      
    case kDistMode4:
      mDistMode[3]=GetParam(kDistMode4)->Value();
      break;
      
    case kSpectBypass:
      mSpectBypass=GetParam(kSpectBypass)->Value();
      break;
      
    case kSolo1:
      mSolo[0]=GetParam(kSolo1)->Value();
      if(mSolo[0]){
        mSolo[1]=false;
        mSolo[2]=false;
        mSolo[3]=false;
        mSoloControl2->SetValueFromPlug(false);
        mSoloControl3->SetValueFromPlug(false);
        mSoloControl4->SetValueFromPlug(false);
        
        mMuteControl1->SetValueFromPlug(false);
      }
      break;
      
    case kSolo2:
      mSolo[1]=GetParam(kSolo2)->Value();
      if(mSolo[1]){
        mSolo[0]=false;
        mSolo[2]=false;
        mSolo[3]=false;
        mSoloControl1->SetValueFromPlug(false);
        mSoloControl3->SetValueFromPlug(false);
        mSoloControl4->SetValueFromPlug(false);
        
        mMuteControl2->SetValueFromPlug(false);
      }
      break;
      
    case kSolo3:
      mSolo[2]=GetParam(kSolo3)->Value();
      if(mSolo[2]){
        mSolo[0]=false;
        mSolo[1]=false;
        mSolo[3]=false;
        mSoloControl1->SetValueFromPlug(false);
        mSoloControl2->SetValueFromPlug(false);
        mSoloControl4->SetValueFromPlug(false);
        
        mMuteControl3->SetValueFromPlug(false);
      }
      break;
      
    case kSolo4:
      mSolo[3]=GetParam(kSolo4)->Value();
      if(mSolo[3]){
        mSolo[0]=false;
        mSolo[1]=false;
        mSolo[2]=false;
        mSoloControl1->SetValueFromPlug(false);
        mSoloControl2->SetValueFromPlug(false);
        mSoloControl3->SetValueFromPlug(false);
        
        mMuteControl4->SetValueFromPlug(false);
      }
      break;
      
    case kMute1:
      mMute[0]=GetParam(kMute1)->Value();
      break;
      
    case kMute2:
      mMute[1]=GetParam(kMute2)->Value();
      break;
      
    case kMute3:
      mMute[2]=GetParam(kMute3)->Value();
      break;
      
    case kMute4:
      mMute[3]=GetParam(kMute4)->Value();
      break;
    
    case kCrossoverFreq1:
      mCrossoverFreq1=GetParam(kCrossoverFreq1)->Value();
      band1lp->setCutoff(mCrossoverFreq1);
      band2hp->setCutoff(mCrossoverFreq1);
      break;
      
    case kCrossoverFreq2:
      mCrossoverFreq2=GetParam(kCrossoverFreq2)->Value();
      band2lp->setCutoff(mCrossoverFreq2);
      band3hp->setCutoff(mCrossoverFreq2);
      break;
      
    case kCrossoverFreq3:
      mCrossoverFreq3=GetParam(kCrossoverFreq3)->Value();
      band3lp->setCutoff(mCrossoverFreq3);
      band4hp->setCutoff(mCrossoverFreq3);
      break;

    default:
      break;
  }
}

double MultibandDistortion::fastAtan(double x){
  return (x / (1.0 + 0.28 * (x * x)));
}