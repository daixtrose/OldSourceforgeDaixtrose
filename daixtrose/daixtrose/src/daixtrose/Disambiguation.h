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

#ifndef DAIXT_DISAMBIGUATION_INC
#define DAIXT_DISAMBIGUATION_INC

////////////////////////////////////////////////////////////////////////////////
// Disambiguation inhibits operations between incompatible types 
////////////////////////////////////////////////////////////////////////////////

#include "daixtrose/CompileTimeChecks.h"

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/identity.hpp"

namespace Daixt 
{

////////////////////////////////////////////////////////////////////////////////
// struct Disambiguator provides one single feature: a typedef named 
// Disambiguation. By inheriting from Disambiguator, every class can obtain
// this feature.

template <class T> 
struct Disambiguator 
{
  enum { is_specialized = false };
  typedef T Disambiguation;
};

////////////////////////////////////////////////////////////////////////////////
// In order to allow the (type of) disambiguation to change, like e.g.
// for matrix * vector -> vector, the arguments of operations are "piped" 
// through BinOpResultDisambiguator and UnOpResultDisambiguator.

// Specialize these ones to achieve special behaviour

////////////////////////////////////////////////////////////////////////////////
// Binops
template <class T1, class T2, class Op> struct BinOpResultDisambiguator;

// For normal operations between types which belong to the same disambiguation
// the disambiguation does not change, thus this default
template <class T, class Op> 
struct BinOpResultDisambiguator<T, T, Op> 
{
  typedef T Disambiguation;
}; 

////////////////////////////////////////////////////////////////////////////////
// UnOps
template <class T, class Op> 
struct UnOpResultDisambiguator
{
  typedef T Disambiguation;
};


namespace Check
{


////////////////////////////////////////////////////////////////////////////////
// compile time check for a disambiguation 
template<class T> 
class has_member_disambiguation // Thanks to Paul Mensonides
{
protected:
  template<class U> static char check(typename U::Disambiguation*);
  template<class U> static char (& check(...))[2];

public:
  // the extra qualifiers help g++-3.4-20030603
  // enum { value = (sizeof(check<T>(0)) == 1) };
  enum { value = (sizeof(has_member_disambiguation::template check<T>(0)) == 1) };
};



template<class T> 
struct has_external_disambiguation 
{
  enum { value = Disambiguator<T>::is_specialized };
};


template<class T> 
struct use_member_disambiguation 
{
  typedef typename T::Disambiguation type;
};

template<class T> 
struct use_external_disambiguation 
{
  typedef typename Disambiguator<T>::Disambiguation type;
};


} // namespace Check  

template<class T> 
struct disambiguation 
{
  typedef typename boost::mpl::eval_if
  <
    Check::has_member_disambiguation<T>,
    Check::use_member_disambiguation<T>,
    boost::mpl::eval_if
    <
     Check::has_external_disambiguation<T>,
     Check::use_external_disambiguation<T>,
     boost::mpl::identity<T>
    >
  >::type type;
};


} // namespace Daixt


#endif // DAIXT_DISAMBIGUATION_INC
