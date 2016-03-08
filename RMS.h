//
//  RMS.h
//
//  Created by Michael Donovan on 3/8/16.
//
//

#ifndef RMS_h
#define RMS_h

class RMSFollower
{
public:
    RMSFollower(double windowSizeMS, double sampleRate){
        windowSize=windowSizeMS/(1000/sampleRate);
        buffer = new double[windowSize];
        sum=0;
    }
    
    ~RMSFollower(){
        if(buffer){
            delete[] buffer;
        }
    };
    
    double getRMS(double sample){
        double RMSLevel;
        for (int i=0; i<windowSize-2; i++) {
            buffer[i]=buffer[i+1];
        }
        buffer[windowSize-1]=sample*sample;
        sum+=buffer[windowSize-1]-buffer[0];
        RMSLevel = sum/10;
        return sqrt(RMSLevel);
    }
    
private:
    double sum;
    int windowSize;
    double* buffer;
};

#endif /* RMS_h */
