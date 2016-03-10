/**
 * \file SecondOrderFilter.cpp
 * @see http://abvolt.com/research/publications2.htm
 * @see http://www.music.mcgill.ca/~ich/classes/FiltersChap2.pdf for the allpass filter
 */

#include "SecondOrderFilter.h"
#include "IIRFilter.h"

#include <cassert>
#include <cmath>

#include <boost/math/constants/constants.hpp>

namespace ATK
{
  template <typename DataType>
  BaseSecondOrderCoefficients<DataType>::BaseSecondOrderCoefficients(int nb_channels)
    :Parent(nb_channels, nb_channels)
  {
  }

  template <typename DataType>
  void BaseSecondOrderCoefficients<DataType>::setup()
  {
    Parent::setup();

    coefficients_in.assign(in_order+1, 0);
    coefficients_out.assign(out_order, 0);
  }

  template <typename DataType_>
  void BaseSecondOrderCoefficients<DataType_>::set_cut_frequency(DataType_ cut_frequency)
  {
    this->cut_frequency = cut_frequency;
    setup();
  }
  
  template <typename DataType_>
  DataType_ BaseSecondOrderCoefficients<DataType_>::get_cut_frequency() const
  {
    return cut_frequency;
  }

  template<typename DataType>
  BandPassCoefficients<DataType>::BandPassCoefficients(int nb_channels)
    :Parent(nb_channels)
  {
  }

  template <typename DataType>
  void BandPassCoefficients<DataType>::setup()
  {
    Parent::setup();
    
    DataType c = std::tan(boost::math::constants::pi<DataType>() * cut_frequency / input_sampling_rate);
    DataType d = (1 + std::sqrt(static_cast<DataType>(2.)) * c + c * c);
    DataType Q_inv = 1 / Q;
    
    coefficients_in[2] = Q_inv * c / d;
    coefficients_in[1] = 0;
    coefficients_in[0] = -Q_inv * c / d;
    coefficients_out[1] = - 2 * (c * c - 1) / d;
    coefficients_out[0] = - (1 - std::sqrt(static_cast<DataType>(2.)) * c + c * c) / d;
  }
  
  template <typename DataType_>
  void BandPassCoefficients<DataType_>::set_Q(DataType_ Q)
  {
    this->Q = Q;
    setup();
  }

  template <typename DataType_>
  DataType_ BandPassCoefficients<DataType_>::get_Q() const
  {
    return Q;
  }

  template<typename DataType>
  LowPassCoefficients<DataType>::LowPassCoefficients(int nb_channels)
    :Parent(nb_channels)
  {
  }

  template <typename DataType>
  void LowPassCoefficients<DataType>::setup()
  {
    Parent::setup();

    DataType c = std::tan(boost::math::constants::pi<DataType>() * cut_frequency / input_sampling_rate);
    DataType d = (1 + std::sqrt(static_cast<DataType>(2.)) * c + c * c);
    
    coefficients_in[2] = c * c / d;
    coefficients_in[1] = 2 * c * c / d;
    coefficients_in[0] = c * c / d;
    coefficients_out[1] = - 2 * (c * c - 1) / d;
    coefficients_out[0] = - (1 - std::sqrt(static_cast<DataType>(2.)) * c + c * c) / d;
  }

  template<typename DataType>
  HighPassCoefficients<DataType>::HighPassCoefficients(int nb_channels)
    :Parent(nb_channels)
  {
  }

  template <typename DataType>
  void HighPassCoefficients<DataType>::setup()
  {
    Parent::setup();

    DataType c = std::tan(boost::math::constants::pi<DataType>() * cut_frequency / input_sampling_rate);
    DataType d = (1 + std::sqrt(static_cast<DataType>(2.)) * c + c * c);
    
    coefficients_in[2] = 1;
    coefficients_in[1] = -2;
    coefficients_in[0] = 1;
    coefficients_out[1] = - 2 * (c * c - 1) / d;
    coefficients_out[0] = - (1 - std::sqrt(static_cast<DataType>(2.)) * c + c * c) / d;
  }

  template<typename DataType>
  BandPassPeakCoefficients<DataType>::BandPassPeakCoefficients(int nb_channels)
    :Parent(nb_channels)
  {
  }

  template <typename DataType>
  void BandPassPeakCoefficients<DataType>::setup()
  {
    Parent::setup();

    DataType c = std::tan(boost::math::constants::pi<DataType>() * cut_frequency / input_sampling_rate);
    DataType Q_inv = 1 / Q;
    if(gain <= 1)
    {
      DataType V0 = 1 / gain;
      DataType d = 1 + V0 * Q_inv * c + c * c;
      
      coefficients_in[2] = (1 + Q_inv * c + c * c) / d;
      coefficients_in[1] = 2 * (c * c - 1) / d;
      coefficients_in[0] = (1 - Q_inv * c + c * c) / d;
      coefficients_out[1] = -2 * (c * c - 1) / d;
      coefficients_out[0] = -(1 - V0 * Q_inv * c + c * c) / d;
    }
    else
    {
      DataType V0 = gain;
      DataType d = 1 + Q_inv * c + c * c;
      
      coefficients_in[2] = (1 + V0 * Q_inv * c + c * c) / d;
      coefficients_in[1] = 2 * (c * c - 1) / d;
      coefficients_in[0] = (1 - V0 * Q_inv * c + c * c) / d;
      coefficients_out[1] = -2 * (c * c - 1) / d;
      coefficients_out[0] = -(1 - Q_inv * c + c * c) / d;
    }
  }

  template <typename DataType_>
  void BandPassPeakCoefficients<DataType_>::set_Q(DataType_ Q)
  {
    this->Q = Q;
    setup();
  }

  template <typename DataType_>
  DataType_ BandPassPeakCoefficients<DataType_>::get_Q() const
  {
    return Q;
  }

  template <typename DataType_>
  void BandPassPeakCoefficients<DataType_>::set_gain(DataType_ gain)
  {
    this->gain = gain;
    setup();
  }

  template <typename DataType_>
  DataType_ BandPassPeakCoefficients<DataType_>::get_gain() const
  {
    return gain;
  }

  template<typename DataType>
  AllPassCoefficients<DataType>::AllPassCoefficients(int nb_channels)
    :Parent(nb_channels)
  {
  }

  template <typename DataType>
  void AllPassCoefficients<DataType>::setup()
  {
    Parent::setup();

    DataType c = std::tan(boost::math::constants::pi<DataType>() * Q);
    DataType d = -std::cos(2 * boost::math::constants::pi<DataType>() * cut_frequency / input_sampling_rate);

    coefficients_in[2] = -c;
    coefficients_in[1] = d * (1 - c);
    coefficients_in[0] = 1;
    coefficients_out[1] = -d * (1 - c);
    coefficients_out[0] = c;
  }

  template <typename DataType_>
  void AllPassCoefficients<DataType_>::set_Q(DataType_ Q)
  {
    this->Q = Q;
    setup();
  }

  template <typename DataType_>
  DataType_ AllPassCoefficients<DataType_>::get_Q() const
  {
    return Q;
  }

  template<typename DataType>
  LowShelvingCoefficients<DataType>::LowShelvingCoefficients(int nb_channels)
    :Parent(nb_channels)
  {
  }

  template <typename DataType>
  void LowShelvingCoefficients<DataType>::setup()
  {
    Parent::setup();

    DataType c = std::tan(boost::math::constants::pi<DataType>() * cut_frequency / input_sampling_rate);
    if(gain <= 1)
    {
      DataType V0 = 1 / gain;
      DataType d = (1 + std::sqrt(static_cast<DataType>(2.) * V0) * c + V0 * c * c);
      
      coefficients_in[2] = (1 + std::sqrt(static_cast<DataType>(2.)) * c + c * c) / d;
      coefficients_in[1] = 2 * (c * c - 1) / d;
      coefficients_in[0] = (1 - std::sqrt(static_cast<DataType>(2.)) * c + c * c) / d;
      coefficients_out[1] = - 2 * (V0 * c * c - 1) / d;
      coefficients_out[0] = - (1 - std::sqrt(static_cast<DataType>(2.) * V0) * c + V0 * c * c) / d;
    }
    else
    {
      DataType d = (1 + std::sqrt(static_cast<DataType>(2.)) * c + c * c);
      
      coefficients_in[2] = (1 + std::sqrt(static_cast<DataType>(2.) * gain) * c + gain * c * c) / d;
      coefficients_in[1] = 2 * (gain * c * c - 1) / d;
      coefficients_in[0] = (1 - std::sqrt(static_cast<DataType>(2.) * gain) * c + gain * c * c) / d;
      coefficients_out[1] = - 2 * (c * c - 1) / d;
      coefficients_out[0] = - (1 - std::sqrt(static_cast<DataType>(2.)) * c + c * c) / d;
    }
  }

  template <typename DataType_>
  void LowShelvingCoefficients<DataType_>::set_gain(DataType_ gain)
  {
    this->gain = gain;
    setup();
  }

  template <typename DataType_>
  DataType_ LowShelvingCoefficients<DataType_>::get_gain() const
  {
    return gain;
  }

  template<typename DataType>
  HighShelvingCoefficients<DataType>::HighShelvingCoefficients(int nb_channels)
    :Parent(nb_channels)
  {
  }

  template <typename DataType>
  void HighShelvingCoefficients<DataType>::setup()
  {
    Parent::setup();

    DataType c = std::tan(boost::math::constants::pi<DataType>() * cut_frequency / input_sampling_rate);
    if(gain <= 1)
    {
      DataType V0 = 1 / gain;
      DataType d = (V0 + std::sqrt(static_cast<DataType>(2.) * V0) * c + c * c);
      
      coefficients_in[2] = -(1 + std::sqrt(static_cast<DataType>(2.0)) * c + c * c) / d;
      coefficients_in[1] = -2 * (c * c - 1) / d;
      coefficients_in[0] = -(1 - std::sqrt(static_cast<DataType>(2.0)) * c + c * c) / d;
      coefficients_out[1] = - 2 * (c * c - V0) / d;
      coefficients_out[0] = - (V0 - std::sqrt(static_cast<DataType>(2.0) * V0) * c + c * c) / d;
    }
    else
    {
      DataType d = (1 + std::sqrt(static_cast<DataType>(2.)) * c + c * c);
      
      coefficients_in[2] = -(gain + std::sqrt(static_cast<DataType>(2.0) * gain) * c + c * c) / d;
      coefficients_in[1] = -2 * (c * c - gain) / d;
      coefficients_in[0] = -(gain - std::sqrt(static_cast<DataType>(2.0) * gain) * c + c * c) / d;
      coefficients_out[1] = - 2 * (c * c - 1) / d;
      coefficients_out[0] = - (1 - std::sqrt(static_cast<DataType>(2.0)) * c + c * c) / d;
    }
  }
  
  template<typename DataType_>
  void HighShelvingCoefficients<DataType_>::set_gain(DataType_ gain)
  {
    this->gain = gain;
    setup();
  }

  template <typename DataType_>
  DataType_ HighShelvingCoefficients<DataType_>::get_gain() const
  {
    return gain;
  }

  template class BaseSecondOrderCoefficients<float>;
  template class BaseSecondOrderCoefficients<double>;
  
  template class BandPassCoefficients<float>;
  template class BandPassCoefficients<double>;
  template class LowPassCoefficients<float>;
  template class LowPassCoefficients<double>;
  template class HighPassCoefficients<float>;
  template class HighPassCoefficients<double>;
  template class BandPassPeakCoefficients<float>;
  template class BandPassPeakCoefficients<double>;
  template class AllPassCoefficients<float>;
  template class AllPassCoefficients<double>;
  template class LowShelvingCoefficients<float>;
  template class LowShelvingCoefficients<double>;
  template class HighShelvingCoefficients<float>;
  template class HighShelvingCoefficients<double>;
  
  template class IIRFilter<BandPassCoefficients<float> >;
  template class IIRFilter<BandPassCoefficients<double> >;
  template class IIRFilter<LowPassCoefficients<float> >;
  template class IIRFilter<LowPassCoefficients<double> >;
  template class IIRFilter<HighPassCoefficients<float> >;
  template class IIRFilter<HighPassCoefficients<double> >;
  template class IIRFilter<BandPassPeakCoefficients<float> >;
  template class IIRFilter<BandPassPeakCoefficients<double> >;
  template class IIRFilter<AllPassCoefficients<float> >;
  template class IIRFilter<AllPassCoefficients<double> >;
  template class IIRFilter<LowShelvingCoefficients<float> >;
  template class IIRFilter<LowShelvingCoefficients<double> >;
  template class IIRFilter<HighShelvingCoefficients<float> >;
  template class IIRFilter<HighShelvingCoefficients<double> >;
}
