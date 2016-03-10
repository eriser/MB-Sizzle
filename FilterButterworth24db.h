
//  by neotec from the music-dsp archive


#ifndef Butterworth_h
#define Butterworth_h

#pragma once

class CFilterButterworth24db
{
public:
    CFilterButterworth24db(void);
    ~CFilterButterworth24db(void);
    void SetSampleRate(double fs);
    void Set(double cutoff, double q);
    double process(double input);
    
private:
    double fastAtan(double x);
    double t0, t1, t2, t3;
    double coef0, coef1, coef2, coef3;
    double history1, history2, history3, history4;
    double gain;
    double min_cutoff, max_cutoff;
};

#endif
