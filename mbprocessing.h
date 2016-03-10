//
//  mbprocessing.h
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

#ifndef mbprocessing_hpp
#define mbprocessing_hpp


#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

class MBProcessing {
public:
    // Default constructors
    MBProcessing();
    ~MBProcessing();
    
    // Sets sample rate, filter order, initializes buffers, output array
    void initMB(const double sr, const double filt_order, const int num_bands);
    
    //  Updates the filter crossovers
    void setMB(const double * crossovers_in);
    
    //  Takes a sample, puts it at the top of the input buffer, calcs output samps and returns in array of length bands
    //  values returned from lowest to highest band
    double * processSamp(const double samp);
    
    
private:
    
    //  Calculates the filter coefficients from the crossovers,
    //  called from setMB
    void calcCoefficients();
    void placeInBuffer(double samp);
    
    double sinc(double x);
    double hamming(int n);
    
    //  Constants:
    double fs;  //  Sample Rate
    int M;   //  Filter Order (needs M+1 samples/taps)
    int bands;  //  Number of bands
    int curPos;    //  Circular buffer end index
    
    double * crossovers;    //  Crossover frequency array
    
    double * * coeffs;  //  2D coefficient array, 1stD is band, 2nd is coeffs
    
    double * invalues;
    double * sampsout;
    
    
    
};




#endif /* mbprocessing_hpp */


