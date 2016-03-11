//
//  PeakFollower.hpp
//  
//
//

#ifndef PeakFollower_hpp
#define PeakFollower_hpp

class PeakFollower {
public:
    PeakFollower(double sampleRate);
    ~PeakFollower();
    float processBand1(double input);
    float processBand2(double input);
    float processBand3(double input);
    float processBand4(double input);

protected:
    float scalar;
};



#endif /* PeakFollower_h */

