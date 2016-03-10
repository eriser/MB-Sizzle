/**
 * \file GainSwellFilter.cpp
 */

#include "GainSwellFilter.h"

#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace ATK
{
  template<typename DataType_>
  GainSwellFilter<DataType_>::GainSwellFilter(int nb_channels, size_t LUTsize, size_t LUTprecision)
  :Parent(nb_channels, LUTsize, LUTprecision), softness(static_cast<DataType_>(.0001))
  {
    recomputeLUT();
  }
  
  template<typename DataType_>
  GainSwellFilter<DataType_>::~GainSwellFilter()
  {
    //Future has to be deleted in child destructor as it uses computeGain
    if(recomputeFuture.valid())
    {
      recomputeFuture.wait();
    }
  }

  template<typename DataType_>
  void GainSwellFilter<DataType_>::set_softness(DataType_ softness)
  {
    if (softness <= 0)
    {
      throw std::out_of_range("Softness factor must be strictly positive value");
    }
    this->softness = softness;
    start_recomputeLUT();
  }
  
  template<typename DataType_>
  DataType_ GainSwellFilter<DataType_>::get_softness() const
  {
    return softness;
  }

  template<typename DataType_>
  DataType_ GainSwellFilter<DataType_>::computeGain( DataType_ value ) const
  {
    if(value == 0)
      return 1;
    DataType diff = 10 * std::log10(value);
    return static_cast<DataType>(std::pow(10, -(std::sqrt(diff*diff + softness) + diff) / 40 * ratio));
  }

  template class GainSwellFilter<float>;
  template class GainSwellFilter<double>;
}
