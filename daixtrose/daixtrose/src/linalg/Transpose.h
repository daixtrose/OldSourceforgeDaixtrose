//-*-C++-*- 
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

#ifndef DAIXT_LINALG_TRANSPOSE_INC
#define DAIXT_LINALG_TRANSPOSE_INC


#include "linalg/RowAndColumCounters.h"
#include "linalg/RowAndColumExtractors.h"
#include "linalg/Disambiguation.h"
#include "linalg/Matrix.h"


namespace Linalg
{
#define TINYMAT_EMPTY_ARG DoNothing

DAIXT_DEFINE_UNOP_AND_FUNCTION(Transpose, TransposeOfMatrix, TINYMAT_EMPTY_ARG)

// Row and operators return the column operators and vice versa

template<class T, class ARG>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, TransposeOfMatrix> >
{
  static inline 
  size_t Apply(const Daixt::UnOp<ARG, TransposeOfMatrix>& arg)
  { 
    return ColumnCounter<MatrixExpression<T> >()(arg.arg());
  }
};


template<class T, class ARG>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, TransposeOfMatrix> >
{
  static inline 
  size_t Apply(const Daixt::UnOp<ARG, TransposeOfMatrix>& arg)
  { 
    return RowCounter<MatrixExpression<T> >()(arg.arg());
  }
};


template<class T, class ARG>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, TransposeOfMatrix> >
{
  static inline 
  typename T::RowStorage
  Apply(const Daixt::UnOp<ARG, TransposeOfMatrix>& arg,
        std::size_t i)
  { 
    return ColExtractor<MatrixExpression<T> >(i)(arg.arg());
  }
};
  

template<class T, class ARG>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, TransposeOfMatrix> >
{
  static inline 
 
  typename T::RowStorage
  Apply(const Daixt::UnOp<ARG, TransposeOfMatrix>& arg,
        std::size_t i)
  { 
    return RowExtractor<MatrixExpression<T> >(i)(arg.arg());
  }
};


} // namespace Linalg



#endif // DAIXT_LINALG_TRANSPOSE_INC
