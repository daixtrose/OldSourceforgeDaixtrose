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

#ifndef DAIXT_COMPILE_TIME_CHECKS_INC
#define DAIXT_COMPILE_TIME_CHECKS_INC

#include "loki/Typelist.h"

namespace Daixt 
{

namespace Check
{
////////////////////////////////////////////////////////////////////////////////
// a helper to distict 2 types
template <class T1, class T2> 
struct Is { static const bool SameType = false; };

template <class T> 
struct Is<T, T> { static const bool SameType = true; };

#define SAME_TYPE(T1, T2) Daixt::Check::Is<T1, T2>::SameType 


////////////////////////////////////////////////////////////////////////////////
// find a type in a typelist and return true if its there
template <class T, class T2> struct Find;

template <class T, class Head, class Tail>
struct Find<T, Loki::Typelist<Head, Tail> > 
{
  static const bool Result = 
    (Loki::TL::IndexOf<Loki::Typelist<Head, Tail>, T>::value != -1);
};

#define FIND(T, L) Daixt::Check::Find<T, L >::Result

////////////////////////////////////////////////////////////////////////////////
// compile time assertion
// FIXIT: Note: this is alpha. Should be changed to use boost checks  
template <bool b> struct CompileTimeAssertion;
template <> struct CompileTimeAssertion<true> {};

#define CONCAT( X, Y ) DO_CONCAT( X, Y )
#define DO_CONCAT( X, Y ) X##Y

#define COMPILE_TIME_ASSERT(ARG) \
  enum CONCAT(compile_time_assertion_failed_in_line_, __LINE__) { \
  CONCAT(Error_, __LINE__) = \
  sizeof(Daixt::Check::CompileTimeAssertion<ARG>) }

} // namespace Check  
} // namespace Daixt 



#endif // DAIXT_COMPILE_TIME_CHECKS_INC
