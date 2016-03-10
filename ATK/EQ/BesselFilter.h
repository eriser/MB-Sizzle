/**
 * \file BesselFilter.h
 */

#ifndef ATK_EQ_BESSELFILTER_H
#define ATK_EQ_BESSELFILTER_H

#include <ATK/Core/TypedBaseFilter.h>

namespace ATK
{
  template<typename DataType_>
  class BesselLowPassCoefficients: public TypedBaseFilter<DataType_>
  {
  public:
    typedef TypedBaseFilter<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
  protected:
    DataType cut_frequency;
    
    int in_order;
    int out_order;
    
    void setup() override;
    
    std::vector<DataType> coefficients_in;
    std::vector<DataType> coefficients_out;
    
  public:
    BesselLowPassCoefficients(int nb_channels = 1);
    
    void set_cut_frequency(DataType_ cut_frequency);
    DataType_ get_cut_frequency() const;
    
    void set_order(int order);
  };
  
  template<typename DataType_>
  class BesselHighPassCoefficients: public TypedBaseFilter<DataType_>
  {
  public:
    typedef TypedBaseFilter<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
  protected:
    DataType cut_frequency;
    
    int in_order;
    int out_order;
    
    void setup() override;
    
    std::vector<DataType> coefficients_in;
    std::vector<DataType> coefficients_out;
    
  public:
    BesselHighPassCoefficients(int nb_channels = 1);
    
    void set_cut_frequency(DataType_ cut_frequency);
    DataType_ get_cut_frequency() const;
    
    void set_order(int order);
  };
  
  template<typename DataType_>
  class BesselBandPassCoefficients: public TypedBaseFilter<DataType_>
  {
  public:
    typedef TypedBaseFilter<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
  protected:
    std::pair<DataType, DataType> cut_frequencies;
    
    int in_order;
    int out_order;
    
    void setup() override;
    
    std::vector<DataType> coefficients_in;
    std::vector<DataType> coefficients_out;
    
  public:
    BesselBandPassCoefficients(int nb_channels = 1);
    
    void set_cut_frequencies(std::pair<DataType_, DataType_> cut_frequencies);
    void set_cut_frequencies(DataType_ f0, DataType_ f1);
    std::pair<DataType_, DataType_> get_cut_frequencies() const;
    
    void set_order(int order);
  };
  
  template<typename DataType_>
  class BesselBandStopCoefficients: public TypedBaseFilter<DataType_>
  {
  public:
    typedef TypedBaseFilter<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
  protected:
    std::pair<DataType, DataType> cut_frequencies;
    
    int in_order;
    int out_order;
    
    void setup() override;
    
    std::vector<DataType> coefficients_in;
    std::vector<DataType> coefficients_out;
    
  public:
    BesselBandStopCoefficients(int nb_channels = 1);
    
    void set_cut_frequencies(std::pair<DataType_, DataType_> cut_frequencies);
    void set_cut_frequencies(DataType_ f0, DataType_ f1);
    std::pair<DataType_, DataType_> get_cut_frequencies() const;
    
    void set_order(int order);
  };
}

#endif
