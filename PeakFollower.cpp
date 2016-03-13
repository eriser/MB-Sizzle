//
//  PeakFollower.cpp
//  
//
//
#include <math.h>
#include "PeakFollower.h"

const float HALF_LIFE = .5;
const float VERY_SMALL_FLOAT = 0.0001;

// halfLife = time in seconds for output to decay to half value after an impulse

PeakFollower::PeakFollower(double sampleRate){
    scalar = pow( 0.5, 1.0/(HALF_LIFE * sampleRate));
};

float PeakFollower::processBand1(double input){
    static float output = 0.0;
    
    if( input < 0.0 )
        input = -input;  /* Absolute value. */
    
    if ( input >= output )
    {
        /* When we hit a peak, ride the peak to the top. */
        output = input;
    }
    else
    {
        /* Exponential decay of output when signal is low. */
        output = output * scalar;
        /*
         ** When current gets close to 0.0, set current to 0.0 to prevent FP underflow
         ** which can cause a severe performance degradation due to a flood
         ** of interrupts.
         */
        if( output < VERY_SMALL_FLOAT ) output = 0.0;
    }
    return output;
}

float PeakFollower::processBand2(double input){
    static float output1 = 0.0;
    
    if( input < 0.0 )
        input = -input;  /* Absolute value. */
    
    if ( input >= output1 )
    {
        /* When we hit a peak, ride the peak to the top. */
        output1 = input;
    }
    else
    {
        /* Exponential decay of output when signal is low. */
        output1 = output1 * scalar;
        /*
         ** When current gets close to 0.0, set current to 0.0 to prevent FP underflow
         ** which can cause a severe performance degradation due to a flood
         ** of interrupts.
         */
        if( output1 < VERY_SMALL_FLOAT ) output1 = 0.0;
    }
    return output1;
}

float PeakFollower::processBand3(double input){
    static float output2 = 0.0;
    
    if( input < 0.0 )
        input = -input;  /* Absolute value. */
    
    if ( input >= output2 )
    {
        /* When we hit a peak, ride the peak to the top. */
        output2 = input;
    }
    else
    {
        /* Exponential decay of output when signal is low. */
        output2 = output2 * scalar;
        /*
         ** When current gets close to 0.0, set current to 0.0 to prevent FP underflow
         ** which can cause a severe performance degradation due to a flood
         ** of interrupts.
         */
        if( output2 < VERY_SMALL_FLOAT ) output2 = 0.0;
    }
    return output2;
}

float PeakFollower::processBand4(double input){
    static float output3 = 0.0;
    
    if( input < 0.0 )
        input = -input;  /* Absolute value. */
    
    if ( input >= output3 )
    {
        /* When we hit a peak, ride the peak to the top. */
        output3 = input;
    }
    else
    {
        /* Exponential decay of output when signal is low. */
        output3 = output3 * scalar;
        /*
         ** When current gets close to 0.0, set current to 0.0 to prevent FP underflow
         ** which can cause a severe performance degradation due to a flood
         ** of interrupts.
         */
        if( output3 < VERY_SMALL_FLOAT ) output3 = 0.0;
    }
    return output3;
}