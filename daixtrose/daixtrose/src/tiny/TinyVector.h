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

#ifndef TINY_TINY_VECTOR_INC
#define TINY_TINY_VECTOR_INC

#include "daixtrose/Daixt.h"

#include <stddef.h> // for size_t
#include <algorithm>
#include <iosfwd>
#include <iomanip>
#include <functional>



namespace TinyVec
{
////////////////////////////////////////////////////////////////////////////////
// TinyVector
////////////////////////////////////////////////////////////////////////////////

// Disambiguation
template <class T, int n>
struct TinyVectorExpression 
{
  typedef T Type;
  typedef T NumT;
  typedef T numerical_type;
  static const int Dimension = n;
};

////////////////////////////////////////////////////////////////////////////////
// TinyVector

// a simple but speedy implementation of a fixed-sized vector which fits into 
// the Daixt context

template <class T, int n>
class TinyVector
{
public:
  //////////////////////////////////////////////////////////////////////////////
  typedef TinyVectorExpression<T, n> Disambiguation;
  typedef T value_type;

  COMPILE_TIME_ASSERT((n > 0));
  
  //////////////////////////////////////////////////////////////////////////////
  inline TinyVector();             // uninitialized data 
  inline TinyVector(const T& t);   // initialized with some value t
  
  // construct from Daixt::Expr<...>
  template<class A> inline TinyVector(const ::Daixt::Expr<A>& E);
  

  //////////////////////////////////////////////////////////////////////////////
  inline void operator=(const T& t);         // assign a value to all elements
  template<class A> inline void operator=(const Daixt::Expr<A>& E);
  
  inline void operator*=(const T& t);
  inline void operator+=(const T& t);
  inline void operator-=(const T& t);


  typedef TinyVector<T, n> MyOwnT;

  inline void operator+=(const MyOwnT& Other);
  inline void operator-=(const MyOwnT& Other);
  

  //////////////////////////////////////////////////////////////////////////////
  // political incorrect, but this is numerics ... we need to call lapack 
  inline T* data();
  inline const T* data() const;
    
  inline T* begin();
  inline T* end();

  inline const T* begin() const;
  inline const T* end() const;


  //////////////////////////////////////////////////////////////////////////////
  inline T& operator()(size_t j);
  inline T operator()(size_t j) const;

private:
  //////////////////////////////////////////////////////////////////////////////
  inline void RangeCheck(size_t j) const;
  
  // stored the fortran way
  T data_[n]; 
};      


    
////////////////////////////////////////////////////////////////////////////////
//**************************** Implementation ********************************//
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// initialisation 

template <class T, int n> 
TinyVector<T, n>::TinyVector() 
{}

template <class T, int n>
TinyVector<T, n>::TinyVector(const T& t) 
{
  std::fill(data_, data_ + n, t);
}

// ... through expression
// musta forward declare here
template<class T> class GetValue;

template<class T, int n>
template<class A> 
TinyVector<T, n>::TinyVector(const ::Daixt::Expr<A>& E)
{
  // we trust in loop unrollers of modern compilers, no need to damage our brain
  // with static unrollers. Your mileage may vary, but since all loop boundaries
  // are known at compile time this loop is unlikely to see the light of day.
  for (size_t j = 1; j != (n + 1); ++j)
    {
      (*this)(j) = GetValue<T>(j)(E);
    }
}

////////////////////////////////////////////////////////////////////////////////

template<class T, int n>
void 
TinyVector<T, n>::operator=(const T& t)
{
  std::fill(data_, data_ + n, t);
}


template<class T, int n>
template<class A> 
void
TinyVector<T, n>::operator=(const ::Daixt::Expr<A>& E)
{
  if (Daixt::CountOccurrence(E, *this))
    {
      TinyVector<T, n> Tmp(E);
      std::copy(Tmp.data(), Tmp.data() + n, data_); 
    }
  else
    {
      for (size_t j = 1; j != (n + 1); ++j)
        {
          (*this)(j) = GetValue<T>(j)(E);
        }
    }
}


template<class T, int n>
void 
TinyVector<T, n>::operator*=(const T& t)
{
  std::transform(data_, data_ + n, 
                 data_,
                 std::bind1st(std::multiplies<T>(), t));
  
}


template<class T, int n>
void 
TinyVector<T, n>::operator+=(const T& t)
{
  std::transform(data_, data_ + n, 
                 data_,
                 std::bind1st(std::plus<T>(), t));
  
}


template<class T, int n>
void 
TinyVector<T, n>::operator-=(const T& t)
{
  std::transform(data_, data_ + n, 
                 data_,
                 std::bind1st(std::minus<T>(), t));
  
}




template<class T, int n>
void 
TinyVector<T, n>::operator+=(const TinyVector<T, n>& Other)
{
  std::transform(data_, data_ + n, 
                 Other.data(),
                 data_,
                 std::plus<T>());
}


template<class T, int n>
void 
TinyVector<T, n>::operator-=(const TinyVector<T, n>& Other)
{
  std::transform(data_, data_ + n, 
                 Other.data(),
                 data_,
                 std::minus<T>());
}

////////////////////////////////////////////////////////////////////////////////
// direct access to underlying fields for low-level communication with fortran

template <class T, int n>
T* 
TinyVector<T, n>::data() 
{
  return data_; 
}

template <class T, int n>
const T* 
TinyVector<T, n>::data() const 
{ 
  return data_; 
}


template <class T, int n>
T* 
TinyVector<T, n>::begin()
{
  return data_;
}

template <class T, int n>
T* 
TinyVector<T, n>::end()
{
  return data_ + n;
}

template <class T, int n>
const T* 
TinyVector<T, n>::begin() const
{
  return data_;
}

template <class T, int n>
const T* 
TinyVector<T, n>::end() const
{
  return data_ + n;
}


////////////////////////////////////////////////////////////////////////////////
// index operators

template <class T, int n>
T& 
TinyVector<T, n>::operator()(size_t j)
{
  RangeCheck(j);
  return data_[j - 1]; 
}

template <class T, int n>
T TinyVector<T, n>::operator()(size_t j) const
{ 
  RangeCheck(j);
  return data_[j - 1];
}


////////////////////////////////////////////////////////////////////////////////
// range check

template <class T, int n>
void TinyVector<T, n>::RangeCheck(size_t j) const
{
  assert(j < static_cast<size_t>(n) + 1);
  assert(j > 0);
}


////////////////////////////////////////////////////////////////////////////////
//*************************** Output Utility *********************************//
////////////////////////////////////////////////////////////////////////////////

template<class T, int n> 
std::ostream& operator<< (std::ostream& os, const TinyVector<T, n>& V)
{
  for (size_t j = 1; j != (n + 1); ++j) 
    {
      os << std::setw(12) << V(j) << "  ";
    } 
  
  return os;
}


} // namespace TinyVec


#endif // TINY_MAT_TINY_VECTOR_INC




    
