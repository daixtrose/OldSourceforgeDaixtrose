//-*-C++-*- 
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

#ifndef DAIXT_LINALG_INVERSE_INC
#define DAIXT_LINALG_INVERSE_INC


#include "linalg/Lump.h"
#include "tiny/TinyMatAndVec.h"

////////////////////////////////////////////////////////////////////////////////
// Inverse of lumped matrices in the ET context of Daixt

namespace Linalg
{

DAIXT_DEFINE_UNOP_AND_FUNCTION(Inverse, InverseOfMatrix, DoNothing)

template<class T, class ARG>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<Daixt::UnOp<ARG, LumpedMatrix>, InverseOfMatrix> >
{
private:
  template <class TT, int n>
  static TinyMat::TinyQuadraticMatrix<TT, n>
  Inverse(const TinyMat::TinyQuadraticMatrix<TT, n>& M)
  {
    // FIXIT: Assumes off-diagonal entries are 0
    TinyMat::TinyQuadraticMatrix<TT, n> Result = 0;
    for (size_t i = 1; i != n + 1; ++i)
      {
        Result(i, i) = 1.0 / M(i, i);
      }

    return Result;
  }

  template <class TT>
  static TT
  Inverse(const TT& t)
  {
    return 1.0 / t;
  }
  

public:
  static inline 
  typename T::RowStorage
  Apply(const Daixt::UnOp<Daixt::UnOp<ARG, LumpedMatrix>, InverseOfMatrix>& arg,
        std::size_t i)
  { 
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::value_type Pair;  
    typedef typename RowStorage::iterator iterator;  
    typedef typename T::NumT NumT;

    // extract the row from the lumped matrix
    RowStorage Result =
      RowExtractor<MatrixExpression<T> >(i)(arg.arg());

    iterator end = Result.end();
    for (iterator iter = Result.begin(); iter != end; ++iter)
      {
        iter->second = Inverse(iter->second);
      }

    return Result;
  }
};
  

template<class T, class ARG>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<Daixt::UnOp<ARG, LumpedMatrix>, InverseOfMatrix> >
{
  static inline 
  typename T::RowStorage
  Apply(const Daixt::UnOp<Daixt::UnOp<ARG, LumpedMatrix>, InverseOfMatrix>& arg,
        std::size_t i)
  { 
    // It's so Easy! Lumped Matrix has only entries on diagonal.
    // => Delegate to RowExtractor
    return RowExtractor<MatrixExpression<T> >(i)(arg);
  }
};


} // namespace Linalg



#endif // DAIXT_LINALG_INVERSE_INC
