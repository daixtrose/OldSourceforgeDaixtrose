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
// developers of daixt (see e.g. http://daixt.sourceforge.net/)

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU Lesser General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU Lesser General Public License.

#ifndef DAIXT_COUNT_OCCURENCE_INC
#define DAIXT_COUNT_OCCURENCE_INC

#include <cstddef> // for size_t

#include "daixtrose/BinOps.h"
#include "daixtrose/UnOps.h"
#include "daixtrose/CompileTimeChecks.h"

namespace Daixt 
{

////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// static occurence counter: counts the number of distinct types inside an
// expression type
////////////////////////////////////////////////////////////////////////////////


template <class T, class S>
struct StaticOccurrenceCounter
{
  static const size_t Result = Check::Is<T, S>::SameType ? 1 : 0; 
};


template <class ARG, class OP, class S>
struct StaticOccurrenceCounter<Daixt::UnOp<ARG, OP>, S>
{
  typedef Daixt::UnOp<ARG, OP> UO;

  static const size_t Result = 
    Check::Is<UO, S>::SameType ? 1 : StaticOccurrenceCounter<ARG, S>::Result;
};


template <class LHS, class RHS, class OP, class S>
struct StaticOccurrenceCounter<Daixt::BinOp<LHS, RHS, OP>, S>
{
  typedef Daixt::BinOp<LHS, RHS, OP> BO;

  static const size_t Result = 
    Check::Is<BO, S>::SameType ? 1 : (StaticOccurrenceCounter<LHS, S>::Result 
                                      +
                                      StaticOccurrenceCounter<RHS, S>::Result) ;
};

template <class T, class S>
struct StaticOccurrenceCounter<Daixt::Expr<T>, S>
{
  static const size_t Result = 
    Check::Is<Daixt::Expr<T>, S>::SameType ? 
    1 
    : StaticOccurrenceCounter<T, S>::Result;
};

template <class T, class S>
struct StaticOccurrenceCounter<Daixt::ConstRef<T>, S>
{
  static const size_t Result = 
    Check::Is<Daixt::ConstRef<T>, S>::SameType ? 
    1 : 
    StaticOccurrenceCounter<T, S>::Result;
};



////////////////////////////////////////////////////////////////////////////////
// a helper function
template <class T, class S> 
inline size_t CountTypeOccurrence(const T& t, const S& s)
{
  return StaticOccurrenceCounter<T, S>::Result;
}


////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// dynamic occurrence counter: counts the number of times a certain reference 
// occurs inside an expression
////////////////////////////////////////////////////////////////////////////////


namespace Private
{

template <class T1, class T2>
inline bool AdressCompare(const T1& t1, const T2& t2)
{
  return static_cast<const void*>(&t1) == static_cast<const void*>(&t2);
}

} // namespace Private


// just a set of overloads, maybe still incomplete
template <class T, class S>
inline size_t CountOccurrence(const T& t, const S& s)
{
  return Private::AdressCompare(t, s);
} 

template <class ARG, class OP, class S>
inline size_t CountOccurrence(const Daixt::UnOp<ARG, OP>& UO, const S& s)
{
  return Private::AdressCompare(UO, s) ? 1 : CountOccurrence(UO.arg(), s); 
} 

template <class LHS, class RHS, class OP, class S>
inline size_t CountOccurrence(const Daixt::BinOp<LHS, RHS, OP>& BO, const S& s)
{
  return Private::AdressCompare(BO, s) ? 1 : 
    (CountOccurrence(BO.lhs(), s) + CountOccurrence(BO.rhs(), s)); 
}

template <class T, class S>
inline size_t CountOccurrence(const Daixt::Expr<T>& E, const S& s)
{
  return Private::AdressCompare(E, s) ? 1 : CountOccurrence(E.content(), s);
} 

template <class T, class S>
inline size_t CountOccurrence(const Daixt::ConstRef<T>& CR, const S& s)
{
  return Private::AdressCompare(CR, s) ? 1 : 
    CountOccurrence(static_cast<const T&>(CR), s);
}



} // namespace Daixt


#endif
