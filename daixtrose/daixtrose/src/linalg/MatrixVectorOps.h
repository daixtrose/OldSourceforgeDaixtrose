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


#ifndef DAIXT_LINALG_MATRIX_VECTOR_OPS_INC
#define DAIXT_LINALG_MATRIX_VECTOR_OPS_INC


#include "linalg/Disambiguation.h"
#include "linalg/Matrix.h"
#include "linalg/Vector.h"

#include "daixtrose/ConstRef.h"



namespace Linalg
{
template <class T, class RowStorage, class Allocator> class Matrix;
template <class T, class Allocator> class Vector;
}


////////////////////////////////////////////////////////////////////////////////
// we never want Daixt::DefaultOps to copy any matrix or vector. Therefore we
// specialize Daixt::CRefOrVal in such a way that its result is always a const
// reference See daixtrose/DefaultOps.h and daixtrose/ConstRef.h for details

namespace Daixt 
{

template <class T, class RowStorage, class Allocator>
struct CRefOrVal<Linalg::Matrix<T, RowStorage, Allocator> > 
{
  typedef Daixt::ConstRef<Linalg::Matrix<T, RowStorage, Allocator> > Type;
};

template <class T, class Allocator>
struct CRefOrVal<Linalg::Vector<T, Allocator> > 
{
  typedef Daixt::ConstRef<Linalg::Vector<T, Allocator> > Type;
};

}


////////////////////////////////////////////////////////////////////////////////
// Matrix * Vector = Vector

namespace Daixt 
{

template <class MT, class MAllocator, class RowStorage, 
          class VT, class VAllocator>
struct BinOpResultDisambiguator<
  Linalg::MatrixExpression<Linalg::MatrixDisambiguator<MT, 
                                                       RowStorage, 
                                                       MAllocator> >,
  Linalg::VectorExpression<Linalg::VectorDisambiguator<VT, VAllocator> >, 
  Daixt::DefaultOps::BinaryMultiply
  >  
{
  // we assume the vector is the master
  typedef Linalg::VectorExpression<Linalg::VectorDisambiguator<VT, VAllocator> >
  Disambiguation;
}; 

} // namespace Daixt 


////////////////////////////////////////////////////////////////////////////////
// useful additional function which helps defining e.g. unary operators
namespace Linalg
{
// just to keep compiler warnings about empty macro args at a minimum
template <class T> inline const T& DoNothing(const T& t) { return t; }

} // namespace Linalg


#endif // DAIXT_LINALG_MATRIX_VECTOR_OPS_INC
