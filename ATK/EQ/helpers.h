/**
 * \file helpers.h
 */

#ifndef ATK_EQ_HELPERS_H
#define ATK_EQ_HELPERS_H

#include <cmath>
#include <vector>

#include <boost/math/tools/polynomial.hpp>

/// Namespace to build filters based on their zpk description
namespace
{
  /// Transform the Wn=1 low pass analog filter in a Wn=Wn low pass filter
  template<typename DataType>
  void zpk_lp2lp(DataType Wn, std::vector<std::complex<DataType> >& z, std::vector<std::complex<DataType> >& p, DataType& k)
  {
    size_t relative_degree = p.size() - z.size();
    
    for(size_t i = 0; i < z.size(); ++i)
    {
      z[i] *= Wn;
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      p[i] *= Wn;
    }
    
    k *= static_cast<DataType>(std::pow(Wn, relative_degree));
  }

  /// Transform the Wn=1 low pass analog filter in a Wn=Wn, bw=bw band pass filter
  template<typename DataType>
  void zpk_lp2bp(DataType Wn, DataType bw, std::vector<std::complex<DataType> >& z, std::vector<std::complex<DataType> >& p, DataType& k)
  {
    size_t relative_degree = p.size() - z.size();
    
    for(size_t i = 0; i < z.size(); ++i)
    {
      z[i] *= bw/2;
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      p[i] *= bw/2;
    }
    
    std::vector<std::complex<DataType> > zbp;
    std::vector<std::complex<DataType> > pbp;

    for(size_t i = 0; i < z.size(); ++i)
    {
      zbp.push_back(z[i] + std::sqrt(z[i]*z[i] - Wn*Wn));
      zbp.push_back(z[i] - std::sqrt(z[i]*z[i] - Wn*Wn));
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      pbp.push_back(p[i] + std::sqrt(p[i]*p[i] - Wn*Wn));
      pbp.push_back(p[i] - std::sqrt(p[i]*p[i] - Wn*Wn));
    }

    zbp.resize(zbp.size() + relative_degree, 0);
    z.swap(zbp);
    p.swap(pbp);
    
    k *= static_cast<DataType>(std::pow(bw, relative_degree));
  }
  
  /// Transform the Wn=1 low pass analog filter in a Wn=Wn, bw=bw band stop filter
  template<typename DataType>
  void zpk_lp2bs(DataType Wn, DataType bw, std::vector<std::complex<DataType> >& z, std::vector<std::complex<DataType> >& p, DataType& k)
  {
    int relative_degree = static_cast<int>(p.size()) - static_cast<int>(z.size());
  
    std::complex<DataType> f = 1;
    for(size_t i = 0; i < z.size(); ++i)
    {
      f *= - z[i];
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      f /= - p[i];
    }
    k *= f.real();

    for(size_t i = 0; i < z.size(); ++i)
    {
      z[i] = bw / 2 / z[i];
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      p[i] = bw / 2 / p[i];
    }
    
    std::vector<std::complex<DataType> > zbs;
    std::vector<std::complex<DataType> > pbs;
    
    for(size_t i = 0; i < z.size(); ++i)
    {
      zbs.push_back(z[i] + std::sqrt(z[i]*z[i] - Wn*Wn));
      zbs.push_back(z[i] - std::sqrt(z[i]*z[i] - Wn*Wn));
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      pbs.push_back(p[i] + std::sqrt(p[i]*p[i] - Wn*Wn));
      pbs.push_back(p[i] - std::sqrt(p[i]*p[i] - Wn*Wn));
    }
    
    zbs.resize(zbs.size() + relative_degree, std::complex<DataType>(0, Wn));
    zbs.resize(zbs.size() + relative_degree, std::complex<DataType>(0, -Wn));
    z.swap(zbs);
    p.swap(pbs);
  }

  /// Apply a bilinear transform on z, p, k
  template<typename DataType>
  void zpk_bilinear(int fs, std::vector<std::complex<DataType> >& z, std::vector<std::complex<DataType> >& p, DataType& k)
  {
    DataType fs2 = 2 * static_cast<DataType>(fs);
  
    std::complex<DataType> f = 1;
    for(size_t i = 0; i < z.size(); ++i)
    {
      f *= fs2 - z[i];
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      f /= fs2 - p[i];
    }
    k *= f.real();
    
    for(size_t i = 0; i < z.size(); ++i)
    {
      z[i] = (fs2 + z[i]) / (fs2 - z[i]);
    }
    for(size_t i = 0; i < p.size(); ++i)
    {
      p[i] = (fs2 + p[i]) / (fs2 - p[i]);
    }
    
    z.resize(p.size(), -1);
  }

  /// Transforms the z, p, k coefficients in b, a form
  template<typename DataType>
  void zpk2ba(int fs, const std::vector<std::complex<DataType> >& z, const std::vector<std::complex<DataType> >& p, DataType k, boost::math::tools::polynomial<DataType>& b, boost::math::tools::polynomial<DataType>& a)
  {
    b = boost::math::tools::polynomial<DataType>(k);
    
    for(int i = 0; i < z.size(); ++i)
    {
      if(z[i].imag() == 0)
      {
        DataType temp[2] = {-z[i].real(), 1};
        boost::math::tools::polynomial<DataType> poly1(temp, 1);
        b *= poly1;
      }
      else if(z[i].imag() < 0)
      {
        DataType temp[3] = {z[i].real() * z[i].real() + z[i].imag() * z[i].imag(), -2 * z[i].real(), 1};
        boost::math::tools::polynomial<DataType> poly2(temp, 2);
        b *= poly2;
      }
    }
    
    a = boost::math::tools::polynomial<DataType>(1);
    for(size_t i = 0; i < p.size(); ++i)
    {
      if(p[i].imag() == 0)
      {
        DataType temp[2] = {-p[i].real(), 1};
        boost::math::tools::polynomial<DataType> poly1(temp, 1);
        a *= poly1;
      }
      else if (p[i].imag() < 0)
      {
        DataType temp[3] = {p[i].real() * p[i].real() + p[i].imag() * p[i].imag(), -2 * p[i].real(), 1};
        boost::math::tools::polynomial<DataType> poly2(temp, 2);
        a *= poly2;
      }
    }
  }
}

#endif
