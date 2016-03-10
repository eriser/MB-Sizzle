//
//  Butterworth.cpp
//  MultibandDistortion
//
//  Created by Michael on 3/9/16.
//
//

#define BUDDA_Q_SCALE 6.f
#include "math.h"
#include "FilterButterworth24db.h"

CFilterButterworth24db::CFilterButterworth24db(void)
{
    this->history1 = 0.f;
    this->history2 = 0.f;
    this->history3 = 0.f;
    this->history4 = 0.f;
    
    this->SetSampleRate(44100.f);
    this->Set(22050.f, 0.0);
}

CFilterButterworth24db::~CFilterButterworth24db(void)
{
}

void CFilterButterworth24db::SetSampleRate(double fs)
{
    double pi = 4.f * fastAtan(1.f);
    
    this->t0 = 4.f * fs * fs;
    this->t1 = 8.f * fs * fs;
    this->t2 = 2.f * fs;
    this->t3 = pi / fs;
    
    this->min_cutoff = fs * 0.01f;
    this->max_cutoff = fs * 0.45f;
}

void CFilterButterworth24db::Set(double cutoff, double q)
{
    if (cutoff < this->min_cutoff)
        cutoff = this->min_cutoff;
    else if(cutoff > this->max_cutoff)
        cutoff = this->max_cutoff;
    
    if(q < 0.f)
        q = 0.f;
    else if(q > 1.f)
        q = 1.f;
    
    double wp = this->t2 * tan(this->t3 * cutoff);
    double bd, bd_tmp, b1, b2;
    
    q *= BUDDA_Q_SCALE;
    q += 1.f;
    
    b1 = (0.765367f / q) / wp;
    b2 = 1.f / (wp * wp);
    
    bd_tmp = this->t0 * b2 + 1.f;
    
    bd = 1.f / (bd_tmp + this->t2 * b1);
    
    this->gain = bd * 0.5f;
    
    this->coef2 = (2.f - this->t1 * b2);
    
    this->coef0 = this->coef2 * bd;
    this->coef1 = (bd_tmp - this->t2 * b1) * bd;
    
    b1 = (1.847759f / q) / wp;
    
    bd = 1.f / (bd_tmp + this->t2 * b1);
    
    this->gain *= bd;
    this->coef2 *= bd;
    this->coef3 = (bd_tmp - this->t2 * b1) * bd;
}

double CFilterButterworth24db::process(double input)
{
    double output = input * this->gain;
    double new_hist;
    
    output -= this->history1 * this->coef0;
    new_hist = output - this->history2 * this->coef1;
    
    output = new_hist + this->history1 * 2.f;
    output += this->history2;
    
    this->history2 = this->history1;
    this->history1 = new_hist;
    
    output -= this->history3 * this->coef2;
    new_hist = output - this->history4 * this->coef3;
    
    output = new_hist + this->history3 * 2.f;
    output += this->history4;
    
    this->history4 = this->history3;
    this->history3 = new_hist;
    
    return output;
}

double CFilterButterworth24db::fastAtan(double x){
    return (x / (1.0 + 0.28 * (x * x)));
}

