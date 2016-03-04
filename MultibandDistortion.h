#ifndef __MULTIBANDDISTORTION__
#define __MULTIBANDDISTORTION__

#include "IPlug_include_in_plug_hdr.h"
#include "CParamSmooth.h"
#include "PeakFollower.h"
#include "VAStateVariableFilter.h"
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
  CParamSmooth* mInputGainSmoother;
  CParamSmooth* mOutputGainSmoother;
  double mInputGain;
  double mOutputGain;
  int mDistType;
  int channelCount;
  bool mAutoGainComp;
  bool mOutputClipping;
};

#endif
