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

#ifndef TINYMAT_MATRIX_VECTOR_OPS_INC
#define TINYMAT_MATRIX_VECTOR_OPS_INC

#include "tiny/TinyMatrix.h"
#include "tiny/TinyVector.h"


////////////////////////////////////////////////////////////////////////////////
// we never want Daixt::DefaultOps to copy any matrix. Therefore we specialize
// Daixt::CRefOrVal in such a way that its result is always a const reference
// See daixt/DefaultOps.h and daixt/ConstRef.h for details

namespace Daixt 
{
  template<class T, int n> 
  struct CRefOrVal<TinyMat::TinyQuadraticMatrix<T, n> > 
  {
    typedef Daixt::ConstRef<TinyMat::TinyQuadraticMatrix<T, n> > Type;
  };

  template<class T, int n> 
  struct CRefOrVal<TinyVec::TinyVector<T, n> > 
  {
    typedef Daixt::ConstRef<TinyVec::TinyVector<T, n> > Type;
  };
}

////////////////////////////////////////////////////////////////////////////////
// useful additional unary operators


namespace TinyMat
{

template <class T> inline const T& DoNothing(const T& t) { return t; }

#define TINYMAT_EMPTY_ARG DoNothing

DAIXT_DEFINE_UNOP_AND_FUNCTION(Transpose, TransposeOfTinyMatrix, TINYMAT_EMPTY_ARG)
DAIXT_DEFINE_UNOP_AND_FUNCTION(Lump, LumpedTinyMatrix, TINYMAT_EMPTY_ARG)


} // namespace TinyMat


////////////////////////////////////////////////////////////////////////////////
// Matrix * Vector = Vector

namespace Daixt 
{
  template<class T, int n> 
  struct BinOpResultDisambiguator<TinyMat::TinyQuadraticMatrixExpression<T, n>,
                                  TinyVec::TinyVectorExpression<T, n>, 
                                  Daixt::DefaultOps::BinaryMultiply>  
  {
    typedef TinyVec::TinyVectorExpression<T, n> Disambiguation;
  }; 

} // namespace Daixt 


////////////////////////////////////////////////////////////////////////////////
// Extract a Row/Colum from a TinyMatrix and store it in a TinyVector

namespace TinyMat
{

template<std::size_t i, class T, int n>
TinyVec::TinyVector<T, n>
ExtractRow(const TinyQuadraticMatrix<T, n>& M)
{
  TinyVec::TinyVector<T, n> Result;

  for (size_t j = 1; j != n+1; ++j)
    {
      Result(j) = M(i, j);
    }
  
  return Result;
}

template<std::size_t i, class T, int n>
TinyVec::TinyVector<T, n>
ExtractColumn(const TinyQuadraticMatrix<T, n>& M)
{
  TinyVec::TinyVector<T, n> Result;

  for (size_t j = 1; j != n+1; ++j)
    {
      Result(j) = M(j, i);
    }
  
  return Result;
}

}

#endif
