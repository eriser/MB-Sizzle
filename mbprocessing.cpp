//
//  mbprocessing.cpp
//  mbprocessing
//
//  Created by Justice Reed on 3/8/16.
//  Copyright © 2016 Justice Reed. All rights reserved.
//
//
//  This class provides multiband FIR Linear Phase filtering functionality.
//  An arbitrary number of filters can be specified
//  The filter order in initMB will determine how much the bands overlap, which is constant and linear in frequency
//  initMB will initialize the filter crossovers evenly spaced in the linear domain
//  setMB will then set the crossovers to a specified set
//
//  To actually filter data, call processSamp for each input data sample one at a time
//  It will return an array of length equal to the number of filters, and contain the output
//  samples in order from lowest to highest frequency band
//
//  The output samples will lag (M+1)/2 behind the input
//


#include "mbprocessing.h"



//===========================================================================================================


MBProcessing::MBProcessing()        //default constructor
{
    fs = 44100; //  default sample rate
    M = 255;    //  default filter order
    bands = 4;
    
    crossovers = new double[bands+1];    //crossover array
    crossovers[0] = 0;
    crossovers[1] = 250;
    crossovers[2] = 1500;
    crossovers[3] = 9000;
    crossovers[4] = fs/2;
    
    coeffs = new double*[bands];
    for (int b=0; b<bands; b++)
    {
        coeffs[b] = new double[M+1];  //initialize each filters coeffs
        for (int k=0; k<=M; k++) { coeffs[b][k] = 0; }  // zero them out
    }
    
    invalues =  new double[M+1]; //input array
    for (int i=0; i<=M; i++) { invalues[i] = 0; }
    curPos = 0;
    
    sampsout = new double[bands]; //output array
    
}


//===========================================================================================================


MBProcessing::~MBProcessing()       //default constructor
{
    delete [] crossovers;
    delete [] invalues;
    delete [] sampsout;
}


//===========================================================================================================


void MBProcessing::initMB(const double sr, const double filt_order, const int num_bands)        //initialize
{
    fs = sr;    //initialize constants
    M = filt_order;
    bands = num_bands;
    if (bands == 0) { bands = 1; }
    
    crossovers[bands + 1] = * new double;    //crossover array, initialized linearly spaced in freq
    for (int b=0; b<=bands; b++) { crossovers[b] = (b/bands)*(fs/2); }
    
    coeffs = new double*[bands];
    for (int b=0; b<bands; b++)
    {
        coeffs[b] =  new double[M+1];  //initialize each filters coeffs
        for (int k=0; k<=M; k++) { coeffs[b][k] = 0; }  // zero them out
    }
    
    invalues[M+1] = * new double; //input array
    for (int i=0; i<=M; i++) { invalues[i] = 0; }
    curPos = 0;
    
    sampsout[bands] = * new double; //output array
    
}


//===========================================================================================================


void MBProcessing::setMB(const double * crossovers_in)          //set crossover frequencies, calc coefficients
{
    //  Set first value to 0 for LP
    crossovers[0] = 0;
    //  Save crossover freqs
    for (int b=1; b<bands; b++) { crossovers[b] = crossovers_in[b]; }
    //  Save nyquist for HP
    crossovers[bands] = fs/2;
    
    calcCoefficients();
}


//===========================================================================================================


void MBProcessing::calcCoefficients()                       // calc coefficients from windowed ideal IR
{
    double ideal;
    double windowed;
    
    //calc coefficients for each band, from lowest to highest
    for (int b=0; b<bands; b++)
    {
        for (int n=0; n<=M; n++)
        {
            ideal = 2*crossovers[b+1]*sinc(2*crossovers[b+1]*(n-M/2)) - 2*crossovers[b]*sinc(2*crossovers[b]*(n-M/2));      //  Ideal BP function
            windowed = ideal*hamming(n);    //  Window with Hamming window
            coeffs[b][M-n] = windowed;      //  Reverse coeffs
        }
    }
}


//===========================================================================================================


double * MBProcessing::processSamp(const double samp)       //  Convolve input samples w/ FIR filters, return outputs
{
    double samp_out;
    int circ;
    
    //  Update input buffer invalues
    placeInBuffer(samp);
    
    //  Calc output sample for each band
    for (int b=0; b<bands; b++)
    {
        //  Multiply each coefficient by the matching input buffer sample
        samp_out = 0;
        for (int n=0; n<=M; n++)
        {
            if ((curPos + n) >= M) { circ = M; }
            else { circ = 0; }
            samp_out += coeffs[b][n]*invalues[curPos + n - circ];
        }
        sampsout[b] = samp_out;
    }
    
    return sampsout;
}


//===========================================================================================================


void MBProcessing::placeInBuffer(double samp)       //  Places latest sample in circular buffer, updates start index
{
    invalues[curPos] = samp;
    if (curPos >= M) { curPos = 0; }
    else curPos++;
    
}


//===========================================================================================================


double MBProcessing::sinc(double x)
{
    //  returns normalized sinc(x)
    if(x == 0.0)
        return 1.0;
    return std::sin(M_PI * x)/(M_PI * x);
}


//===========================================================================================================


double MBProcessing::hamming(int n)
{
    //  returns hamming window of length M+1 at value n
    return (0.54 - 0.46*std::cos(2*M_PI*n/M));
}


//===========================================================================================================





