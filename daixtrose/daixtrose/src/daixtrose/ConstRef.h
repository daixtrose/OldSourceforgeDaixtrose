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

#ifndef DAIXT_CONST_REF_INC
#define DAIXT_CONST_REF_INC

#include "daixtrose/BinOps.h"
#include "daixtrose/UnOps.h"

namespace Daixt
{

////////////////////////////////////////////////////////////////////////////////
// ConstRef: inhibit copying of huge objects, allow value semantics
////////////////////////////////////////////////////////////////////////////////

template <class T> 
class ConstRef
{
 private:
  const T& t_;
 public:
  
  explicit inline ConstRef(const T& t) : t_(t) {}
  operator const T&() const { return t_; }
 
  typedef typename T::Disambiguation Disambiguation;
};


////////////////////////////////////////////////////////////////////////////////
// CRefOrVal 
////////////////////////////////////////////////////////////////////////////////

// users may decide whether they like their own classes to be wrapped into 
// a ConstRef or whether those are cheap enough to be copied around.
// All You have to do is specialize Daixt::CRefOrVal.
// 
// Daixt's default behaviour must be not to wrap into a const ref but rely on
// value semantics

template<class T> 
struct CRefOrVal 
{
  typedef T Type;
};

// again we are paranoid because of stupid library writers like Markus Werle
// (that's me) who may break everything with future hacks

// not defined. Those are NoNos. 
template<class T> struct CRefOrVal<CRefOrVal<T> >; 
template<class LHS, class RHS, class OP> struct CRefOrVal<BinOp<LHS, RHS, OP> >;
template<class ARG, class OP> struct CRefOrVal<UnOp<ARG, OP> >;

} // namespace Daixt

#endif // DAIXT_CONST_REF_INC
