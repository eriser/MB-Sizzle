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
    RMSFollower(){
        sum=0;
    }
    
    ~RMSFollower();
    
    double getRMS(double sample){
        double RMSLevel;
        for (int i=0; i<498; i++) {
            buffer[i]=buffer[i+1];
        }
        buffer[499]=sample*sample;
        sum+=buffer[499]-buffer[0];
        RMSLevel = sum/10;
        return sqrt(RMSLevel);
    }
    
private:
    double sum;
    double buffer[500];
};

#endif /* RMS_h */
