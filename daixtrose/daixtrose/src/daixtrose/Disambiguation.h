//-*-c++-*-
//
// Copyright (C) 2003 Markus Werle
//
// This file is part of the Daixt C++ Library.  This library is free
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

#ifndef DAIXT_DISAMBIGUATION_INC
#define DAIXT_DISAMBIGUATION_INC

////////////////////////////////////////////////////////////////////////////////
// Disambiguation inhibits operations between incompatible types 
////////////////////////////////////////////////////////////////////////////////


namespace Daixt 
{

////////////////////////////////////////////////////////////////////////////////
// struct Disambiguator provides one single feature: a typedef named 
// Disambiguation. By inheriting from Disambiguator, every class can obtain
// this feature.

template <class T> 
struct Disambiguator 
{
  typedef T Disambiguation;
  virtual ~Disambiguator() {} 
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


} // namespace Daixt


#endif // DAIXT_DISAMBIGUATION_INC
