//-*-c++-*-
//
// Copyright (C) 2003 Markus Werle
//
// This file is part of the Daixtrose C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2.1, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.  If not, send mail to the
// developers of daixtrose (see e.g. http://daixtrose.sourceforge.net/)

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU Lesser General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU Lesser General Public License.

#ifndef TINYMAT_TINY_MATRIX_INC
#define TINYMAT_TINY_MATRIX_INC

#include <cstddef> // for std::size_t
#include <algorithm>
#include <iosfwd>
#include <iomanip>
#include <functional>

#include <cstring>
#include <cassert>

#include "daixtrose/Daixt.h"

namespace TinyMat
{
  

////////////////////////////////////////////////////////////////////////////////
// TinyQuadraticMatrix
////////////////////////////////////////////////////////////////////////////////

// Disambiguation
template <class T, std::size_t n>
struct TinyQuadraticMatrixExpression 
{
  typedef T Type;
  static const std::size_t Dimension = n;
};

////////////////////////////////////////////////////////////////////////////////
// TinyQuadraticMatrix

// a simple but speedy implementation of a fixed-sized matrix which fits std::size_to 
// the Daixt context

template <class T, std::size_t n>
class TinyQuadraticMatrix
{
public:
  typedef T value_type;
  // FIXIT : make this class fit for use in std::algos, provide typedefs

  //////////////////////////////////////////////////////////////////////////////
  typedef TinyQuadraticMatrixExpression<T, n> Disambiguation;
  COMPILE_TIME_ASSERT((n > 0));
  
  //////////////////////////////////////////////////////////////////////////////
  inline TinyQuadraticMatrix();             // uninitialized data 
  inline TinyQuadraticMatrix(const T& t);   // initialized with some value t
  
  // construct from Daixt::Expr<...>
  template<class A> inline TinyQuadraticMatrix(const ::Daixt::Expr<A>& E);
  

  //////////////////////////////////////////////////////////////////////////////
  inline void operator=(const T& t);         // assign a value to all elements
  inline void operator=(const TinyQuadraticMatrix<T, n>& Other);
  template<class A> inline void operator=(const Daixt::Expr<A>& E);
  
  inline void operator*=(const T& t);
  inline void operator+=(const T& t);
  inline void operator-=(const T& t);

  inline void operator+=(const TinyQuadraticMatrix<T, n>& Other);
  inline void operator-=(const TinyQuadraticMatrix<T, n>& Other);

  //////////////////////////////////////////////////////////////////////////////
  // political incorrect, but this is numerics ... we need to call lapack 
  inline T* data();
  inline const T* data() const;
    
  //////////////////////////////////////////////////////////////////////////////
  inline T& operator()(size_t i, std::size_t j);
  inline T operator()(size_t i, std::size_t j) const;

  inline std::size_t size() const;

private:
  //////////////////////////////////////////////////////////////////////////////
  inline void RangeCheck(size_t i, std::size_t j) const;
  
  // stored the fortran way
  T data_[n * n]; 
};      


    
////////////////////////////////////////////////////////////////////////////////
//**************************** Implementation ********************************//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// initialisation 

template <class T, std::size_t n> 
TinyQuadraticMatrix<T, n>::TinyQuadraticMatrix() 
{}

template <class T, std::size_t n>
TinyQuadraticMatrix<T, n>::TinyQuadraticMatrix(const T& t) 
{
  std::fill(data_, data_ + (n * n), t);
}

// ... through expression
// musta forward declare here
template<class T> class GetValue;

template<class T, std::size_t n>
template<class A> 
TinyQuadraticMatrix<T, n>::TinyQuadraticMatrix(const ::Daixt::Expr<A>& E)
{
  // we trust in loop unrollers of modern compilers, no need to damage our brain
  // with static unrollers. Your mileage may vary, but since all loop boundaries
  // are known at compile time this loop is unlikely to see the light of day.
  for (size_t j = 1; j != (n + 1); ++j)
    {
      for (size_t i = 1; i != (n + 1); ++i)
        {
          (*this)(i, j) = GetValue<T>(i, j)(E);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

template<class T, std::size_t n>
void 
TinyQuadraticMatrix<T, n>::operator=(const T& t)
{
  std::fill(data_, data_ + n * n, t);
}


template<class T, std::size_t n>
void 
TinyQuadraticMatrix<T, n>::operator*=(const T& t)
{
  std::transform(data_, data_ + n * n, 
                 data_,
                 std::bind1st(std::multiplies<T>(), t));
}


template<class T, std::size_t n>
void 
TinyQuadraticMatrix<T, n>::operator+=(const T& t)
{
  std::transform(data_, data_ + n * n, 
                 data_,
                 std::bind1st(std::plus<T>(), t));
}


template<class T, std::size_t n>
void 
TinyQuadraticMatrix<T, n>::operator-=(const T& t)
{
  std::transform(data_, data_ + n * n, 
                 data_,
                 std::bind1st(std::minus<T>(), t));
}

template<class T, std::size_t n>
void 
TinyQuadraticMatrix<T, n>::
operator+=(const TinyQuadraticMatrix<T, n>& Other)
{
  const T* source = Other.data();
  T* target = data_;
  
  for (size_t i = 0; i < n * n; ++i)
    {
      *target += *source;
      ++target;
      ++source;
    }
}

template<class T, std::size_t n>
void 
TinyQuadraticMatrix<T, n>::
operator-=(const TinyQuadraticMatrix<T, n>& Other)
{
  for (size_t i = 0; i != n * n; ++i)
    {
      *(data_ + i) -= *(Other.data() + i);
    }
}


////////////////////////////////////////////////////////////////////////////////
// we guess that's what the compiler would do if it builds it itself
// added for profiling to show it (maybe)
template<class T, std::size_t n>
void
TinyQuadraticMatrix<T, n>::operator=(const TinyQuadraticMatrix<T, n>& Other)
{
  std::memcpy(data_, Other.data_, n * n * sizeof(T));
}

template<class T, std::size_t n>
template<class A> 
void
TinyQuadraticMatrix<T, n>::operator=(const ::Daixt::Expr<A>& E)
{
  if (Daixt::CountOccurrence(E, *this))
    {
      TinyQuadraticMatrix<T, n> Tmp(E);
      std::copy(Tmp.data(), Tmp.data() + n*n, data_); 
    }
  else
    {
      for (size_t j = 1; j != (n + 1); ++j)
        {
          for (size_t i = 1; i != (n + 1); ++i)
            {
              (*this)(i, j) = GetValue<T>(i, j)(E);
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// direct access to underlying fields for low-level communication with fortran

template <class T, std::size_t n>
T* 
TinyQuadraticMatrix<T, n>::data() {
  return data_; 
}

template <class T, std::size_t n>
const T* 
TinyQuadraticMatrix<T, n>::data() const 
{ 
  return data_; 
}


////////////////////////////////////////////////////////////////////////////////
// index operators

template <class T, std::size_t n>
T& 
TinyQuadraticMatrix<T, n>::operator()(size_t i, std::size_t j)
{
  // column wise storage
  RangeCheck(i, j);
  return data_[i - 1 + (j - 1) * n]; 
}

template <class T, std::size_t n>
T TinyQuadraticMatrix<T, n>::operator()(size_t i, std::size_t j) const
{ 
  // column wise storage
  RangeCheck(i, j);
  return data_[i - 1 + (j - 1) * n]; 
}


template <class T, std::size_t n>
std::size_t
TinyQuadraticMatrix<T, n>::size() const { return n; }


////////////////////////////////////////////////////////////////////////////////
// range check

template <class T, std::size_t n>
void TinyQuadraticMatrix<T, n>::RangeCheck(size_t i, std::size_t j) const
{
  assert (i > 0);
  assert (j > 0);

  assert(i < static_cast<size_t>(n) + 1);
  assert(j < static_cast<size_t>(n) + 1);
}


////////////////////////////////////////////////////////////////////////////////
//*************************** Output Utility *********************************//
////////////////////////////////////////////////////////////////////////////////

template<class T, std::size_t n> 
inline 
std::ostream& operator<< (std::ostream& os, const TinyQuadraticMatrix<T, n>& M)
{
  for (size_t i = 1; i != (n + 1); ++i) 
    {
      for (size_t j = 1; j != (n + 1); ++j) 
        {
          os << std::setw(12) << M(i, j) << "  ";
          //os << M(i, j) << "  ";
        } 
      os << '\n';
    }
  
  os << std::endl;
  return os;
}


////////////////////////////////////////////////////////////////////////////////
//*************************** Row and Colum Setters **************************//
////////////////////////////////////////////////////////////////////////////////

template<class T, std::size_t n> 
inline 
void SetEntriesInRowTo(std::size_t i, T Value, TinyQuadraticMatrix<T, n>& M)
{
  for (std::size_t j = 1; j != n + 1; ++j)
    {
      M(i, j) = Value;
    }
}

template<class T, std::size_t n> 
inline 
void SetEntriesInColTo(std::size_t j, T Value, TinyQuadraticMatrix<T, n>& M)
{
  for (std::size_t i = 1; i != n + 1; ++i)
    {
      M(i, j) = Value;
    }
}



} // namespace TinyMat


#endif // TINY_MATRIX_TINY_MATRIX_INC




    
