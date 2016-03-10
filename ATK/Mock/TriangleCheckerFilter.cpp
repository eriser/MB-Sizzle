/**
 * \file TriangleCheckerFilter.cpp
 */

#include "TriangleCheckerFilter.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

#include <boost/type_traits/is_floating_point.hpp>
#include <boost/utility/enable_if.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

namespace
{
  template<class DataType>
  typename boost::enable_if<typename boost::is_floating_point<DataType>::type>::type compare(DataType v1, DataType v2)
  {
    if(std::abs(v1) < 1e-6 || std::abs(v2) < 1e-5)
    {
      BOOST_REQUIRE_SMALL(v1, static_cast<DataType>(1e-5));
      BOOST_REQUIRE_SMALL(v2, static_cast<DataType>(1e-5));
    }
    else
    {
      BOOST_REQUIRE_CLOSE(v1, v2, 0.0001);
    }
  }

  template<class DataType>
  typename boost::disable_if<typename boost::is_floating_point<DataType>::type>::type compare(DataType v1, DataType v2)
  {
    BOOST_REQUIRE_EQUAL(v1, v2);
  }
}

namespace ATK
{
  template<class DataType_>
  TriangleCheckerFilter<DataType_>::TriangleCheckerFilter()
  :TypedBaseFilter<DataType_>(1, 0), state(0), ascending(true), amplitude(1), frequency(1)
  {
  }
  
  template<class DataType_>
  TriangleCheckerFilter<DataType_>::~TriangleCheckerFilter()
  {
  }
  
  template<class DataType_>
  void TriangleCheckerFilter<DataType_>::set_amplitude(DataType amplitude)
  {
    this->amplitude = amplitude;
  }
  
  template<class DataType_>
  void TriangleCheckerFilter<DataType_>::set_frequency(int frequency)
  {
    this->frequency = frequency;
  }

  template<class DataType_>
  void TriangleCheckerFilter<DataType_>::process_impl(int64_t size) const
  {
    double real_increment = 2. / input_sampling_rate * frequency;
    
    for(int64_t i = 0; i < size; ++i)
    {
      state += real_increment * (ascending ? 1 : -1);
      if(state >= 1)
      {
        state -= 2 * real_increment;
        ascending = !ascending;
      }
      else if(state <= -1)
      {
        state += 2 * real_increment;
        ascending = !ascending;
      }
      compare(converted_inputs[0][i], static_cast<DataType>(amplitude * state));
    }
  }

  template class TriangleCheckerFilter<std::int16_t>;
  template class TriangleCheckerFilter<std::int32_t>;
  template class TriangleCheckerFilter<int64_t>;
  template class TriangleCheckerFilter<float>;
  template class TriangleCheckerFilter<double>;
}
