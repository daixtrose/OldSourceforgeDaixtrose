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


#ifndef DAIXT_LINALG_DISAMBIGUATION_INC
#define DAIXT_LINALG_DISAMBIGUATION_INC

namespace Linalg
{

////////////////////////////////////////////////////////////////////////////////
// These are used for disambiguation. 

template <class T> 
struct VectorExpression {
  typedef typename T::NumT NumT; 
}; 

template <class T> 
struct MatrixExpression {
  typedef typename T::RowStorage RowStorage; // needed in matrix * vector
  typedef typename T::Allocator Allocator;   // - dito -
}; 

template <class T, class RowStorageT, class AllocatorT>
struct MatrixDisambiguator;

template <class T, class AllocatorT>
struct VectorDisambiguator;


}

#endif // DAIXT_LINALG_DISAMBIGUATION_INC
