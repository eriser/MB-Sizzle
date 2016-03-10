/**
 * \file SecondOrderFilter.h
 */

#ifndef ATK_EQ_SECONDORDERFILTER_H
#define ATK_EQ_SECONDORDERFILTER_H

#include <ATK/Core/TypedBaseFilter.h>

namespace ATK
{
  template<typename DataType_>
  class BaseSecondOrderCoefficients: public TypedBaseFilter<DataType_>
  {
  public:
    typedef TypedBaseFilter<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::setup;
  protected:
    DataType cut_frequency;

    const static int in_order=2;
    const static int out_order=2;
    std::vector<DataType> coefficients_in;
    std::vector<DataType> coefficients_out;

    void setup() override;

  public:
    BaseSecondOrderCoefficients(int nb_channels = 1);
    void set_cut_frequency(DataType_ cut_frequency);
    DataType_ get_cut_frequency() const;
  };

  /**
   * Coefficients for a second order bandpass filter
   */
  template<typename DataType_>
  class BandPassCoefficients: public BaseSecondOrderCoefficients<DataType_>
  {
  public:
    typedef BaseSecondOrderCoefficients<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
  protected:
    using Parent::cut_frequency;
    using Parent::coefficients_in;
    using Parent::coefficients_out;
  private:
    DataType Q;
    
  protected:
    void setup() override;
    
  public:
    BandPassCoefficients(int nb_channels = 1);

    void set_Q(DataType_ Q);
    DataType_ get_Q() const;
  };

  /**
   * Coefficients for a second order lowpass filter
   */
  template<typename DataType_>
  class LowPassCoefficients: public BaseSecondOrderCoefficients<DataType_>
  {
  public:
    typedef BaseSecondOrderCoefficients<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
  protected:
    using Parent::cut_frequency;
    using Parent::coefficients_in;
    using Parent::coefficients_out;
    void setup() override;
  public:
    LowPassCoefficients(int nb_channels = 1);
  };
  
  /**
   * Coefficients for a second order highpass filter
   */
  template<class DataType_>
  class HighPassCoefficients: public BaseSecondOrderCoefficients<DataType_>
  {
  public:
    typedef BaseSecondOrderCoefficients<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
  protected:
    using Parent::cut_frequency;
    using Parent::coefficients_in;
    using Parent::coefficients_out;
    void setup() override;
  public:
    HighPassCoefficients(int nb_channels = 1);
  };

  /**
   * Coefficients for a second order bandpass peak filter
   */
  template<typename DataType_>
  class BandPassPeakCoefficients: public BaseSecondOrderCoefficients<DataType_>
  {
  public:
    typedef BaseSecondOrderCoefficients<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
  protected:
    using Parent::cut_frequency;
    using Parent::coefficients_in;
    using Parent::coefficients_out;
  private:
    DataType Q;
    DataType gain;
  protected:
    void setup() override;
    
  public:
    BandPassPeakCoefficients(int nb_channels = 1);

    void set_Q(DataType_ Q);
    DataType_ get_Q() const;
    void set_gain(DataType_ gain);
    DataType_ get_gain() const;
  };

  
  /**
   * Coefficients for a second order allpass filter
   */
  template<typename DataType_>
  class AllPassCoefficients: public BaseSecondOrderCoefficients<DataType_>
  {
  public:
    typedef BaseSecondOrderCoefficients<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
  protected:
    using Parent::cut_frequency;
    using Parent::coefficients_in;
    using Parent::coefficients_out;
  private:
    DataType Q;
  protected:
    void setup() override;
    
  public:
    AllPassCoefficients(int nb_channels = 1);

    /// Q is the allpass relative width (to the sampling rate)
    void set_Q(DataType_ Q);
    DataType_ get_Q() const;
  };

  /**
   * Coefficients for a second order low-pass shelving filter
   */
  template<typename DataType_>
  class LowShelvingCoefficients: public BaseSecondOrderCoefficients<DataType_>
  {
  public:
    typedef BaseSecondOrderCoefficients<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
  protected:
    using Parent::cut_frequency;
    using Parent::coefficients_in;
    using Parent::coefficients_out;
    void setup() override;
  private:
    DataType gain;
    
  public:
    LowShelvingCoefficients(int nb_channels = 1);

    void set_gain(DataType_ gain);
    DataType_ get_gain() const;
  };
  
  /**
   * Coefficients for a second order high-pass shelving filter
   */
  template<typename DataType_>
  class HighShelvingCoefficients: public BaseSecondOrderCoefficients<DataType_>
  {
  public:
    typedef BaseSecondOrderCoefficients<DataType_> Parent;
    using typename Parent::DataType;
    using Parent::input_sampling_rate;
    using Parent::output_sampling_rate;
  protected:
    using Parent::cut_frequency;
    using Parent::coefficients_in;
    using Parent::coefficients_out;
    void setup();
  private:
    DataType gain;
    
  public:
    HighShelvingCoefficients(int nb_channels = 1);

    void set_gain(DataType_ gain);
    DataType_ get_gain() const;
  };
}

#endif
