//
//  PeakFollower.hpp
//  GritDistortion
//
//

#ifndef PeakFollower_hpp
#define PeakFollower_hpp

class PeakFollower {
public:
    PeakFollower();
    ~PeakFollower();
    float process(double input, double sampleRate);
    float process2(double input, double sampleRate);

};



#endif /* PeakFollower_h */

