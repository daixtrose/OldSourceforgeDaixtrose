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


#ifndef DAIXT_LINALG_ROW_SUM_INC
#define DAIXT_LINALG_ROW_SUM_INC


#include "linalg/RowAndColumCounters.h"
#include "linalg/RowAndColumExtractors.h"
#include "linalg/Disambiguation.h"
#include "linalg/Matrix.h"
#include "linalg/Vector.h"


namespace Linalg
{
DAIXT_DEFINE_UNOP_AND_FUNCTION(RowSum, RowSumOfMatrix, DoNothing)
}

namespace Daixt 
{
  template <class T, class RowStorage, class Allocator> 
  struct UnOpResultDisambiguator<
    Linalg::MatrixExpression<Linalg::MatrixDisambiguator<T, 
                                                         RowStorage, 
                                                         Allocator> >,
    Linalg::RowSumOfMatrix>
  {
    typedef 
    Linalg::VectorExpression<Linalg::VectorDisambiguator<T, Allocator> > 
    Disambiguation;
  }; 
} // namespace Daixt 



namespace Linalg
{

template<class T, class ARG>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Daixt::UnOp<ARG, Linalg::RowSumOfMatrix> >
{
  static inline 
  size_t Apply(const Daixt::UnOp<ARG, Linalg::RowSumOfMatrix>& arg)
  { 
    return RowCounter<VectorExpression<T> >()(arg.arg());
  }
};


// template<class T, class ARG>
// struct
// OperatorDelimImpl<ColumnCounter<VectorExpression<T> >, 
//                   Daixt::UnOp<ARG, Linalg::RowSumOfMatrix> >
// {
//   static inline 
//   size_t Apply(const Daixt::UnOp<ARG, Linalg::RowSumOfMatrix>& arg)
//   { 
//     return 1;
//   }
// };

////////////////////////////////////////////////////////////////////////////////
// extract a "row" of RowSum(SomeExpression)

template<class T, class ARG>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::UnOp<ARG, RowSumOfMatrix> >
{
  static inline 
  typename T::NumT
  Apply(const Daixt::UnOp<ARG, RowSumOfMatrix>& arg,
        std::size_t i)
  { 
    typedef typename T::NumT NumT;
    typedef typename ARG::Disambiguation::RowStorage RowStorage;
    typedef typename RowStorage::const_iterator const_iterator;  
    
    // extract the row
    RowStorage Row =
      RowExtractor<typename ARG::Disambiguation>(i)(arg.arg());

    NumT Result = 0;

    const_iterator end = Row.end();
    for (const_iterator iter = Row.begin(); iter != end; ++iter)
      {
        Result += iter->second;
      }
    return Result;
  }
};




} // namespace Linalg


#endif // DAIXT_LINALG_ROW_SUM_INC
