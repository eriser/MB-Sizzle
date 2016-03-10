/**
 * \file GainLimiterFilter.cpp
 */

#include "GainLimiterFilter.h"

#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace ATK
{
  template<typename DataType_>
  GainLimiterFilter<DataType_>::GainLimiterFilter(int nb_channels, size_t LUTsize, size_t LUTprecision)
  :Parent(nb_channels, LUTsize, LUTprecision), softness(static_cast<DataType_>(.0001))
  {
    recomputeLUT();
  }

  template<typename DataType_>
  GainLimiterFilter<DataType_>::~GainLimiterFilter()
  {
    //Future has to be deleted in child destructor as it uses computeGain
    if(recomputeFuture.valid())
    {
      recomputeFuture.wait();
    }
  }

  template<typename DataType_>
  void GainLimiterFilter<DataType_>::set_softness(DataType_ softness)
  {
    if (softness <= 0)
    {
      throw std::out_of_range("Softness factor must be strictly positive value");
    }
    this->softness = softness;
    start_recomputeLUT();
  }
  
  template<typename DataType_>
  DataType_ GainLimiterFilter<DataType_>::get_softness() const
  {
    return softness;
  }

  template<typename DataType_>
  DataType_ GainLimiterFilter<DataType_>::computeGain( DataType_ value ) const
  {
    if(value == 0)
      return 1;
    DataType diff = 10 * std::log10(value);
    return static_cast<DataType>(std::pow(10, -(std::sqrt(diff*diff + softness) + diff) / 40));
  }


  template class GainLimiterFilter<float>;
  template class GainLimiterFilter<double>;
}
