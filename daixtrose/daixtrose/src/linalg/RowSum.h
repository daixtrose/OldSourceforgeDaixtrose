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
// developers of daixtrose (see e.g. http://daixtrose.sourceforge.net/)

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU Lesser General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU Lesser General Public License.


// FIXIT: WORKS ONLY FOR BLOCKED MATRIX

#ifndef DAIXT_LINALG_ROW_SUM_INC
#define DAIXT_LINALG_ROW_SUM_INC


#include "linalg/RowAndColumCounters.h"
#include "linalg/RowAndColumExtractors.h"
#include "linalg/Disambiguation.h"
#include "linalg/Matrix.h"
#include "linalg/Vector.h"

#include "tiny/TinyMatAndVec.h"


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
  Linalg::VectorExpression<Linalg::VectorDisambiguator
  <T, typename RowStorage::allocator_type> > 
  Disambiguation;
}; 


// Block matrices need a special handling (row sums over the block matrices)
template <class T, int N, class RowStorage, class Allocator> 
struct UnOpResultDisambiguator<
  Linalg::MatrixExpression<
    Linalg::MatrixDisambiguator<TinyMat::TinyQuadraticMatrix<T, N>, 
                                RowStorage, 
                                Allocator> >,
    Linalg::RowSumOfMatrix>
{
  typedef 
  Linalg::VectorExpression<Linalg::VectorDisambiguator
  <TinyVec::TinyVector<T, N>, typename RowStorage::allocator_type> > 
  Disambiguation;
}; 

} // namespace Daixt 



namespace Linalg
{

template<class T, class ARG, class Allocator>
struct
OperatorDelimImpl
<
  RowCounter<
             VectorExpression<
                              VectorDisambiguator<T, Allocator> 
                              > 
             >, 
  Daixt::UnOp<ARG, RowSumOfMatrix> 
>
{
  static inline 
  size_t Apply(const Daixt::UnOp<ARG, RowSumOfMatrix>& arg)
  { 
    typedef VectorDisambiguator<T, Allocator> Disambiguation;
    return RowCounter<VectorExpression<Disambiguation> >()(arg.arg());
  }
};


// matrix
template<class T, class VecAllocator, 
         class MatT, class RowStorage, class MatAllocator>
struct
OperatorDelimImpl
<
  RowCounter<
             VectorExpression<
                              VectorDisambiguator<T, VecAllocator> 
                              > 
             >, 
  Linalg::Matrix<MatT, RowStorage, MatAllocator> 
>
{
  static inline size_t Apply
  (const Linalg::Matrix<MatT, RowStorage, MatAllocator>& arg) 
  {
    return arg.nrows();
  }
};





// template<class T, class ARG>
// struct
// OperatorDelimImpl<ColumnCounter<VectorExpression<T> >, 
//                   Daixt::UnOp<ARG, RowSumOfMatrix> >
// {
//   static inline 
//   size_t Apply(const Daixt::UnOp<ARG, RowSumOfMatrix>& arg)
//   { 
//     return 1;
//   }
// };

////////////////////////////////////////////////////////////////////////////////
// extract a "row" of RowSum(SomeExpression)

template<class T, class ARG, class Allocator>
struct
OperatorDelimImpl
<
  RowExtractor<
               VectorExpression<
                                Linalg::VectorDisambiguator<T, Allocator> 
                                > 
               >, 
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


template<class T, int N, class ARG, class Allocator>
struct
OperatorDelimImpl
<
  RowExtractor<
               VectorExpression<
                                Linalg::VectorDisambiguator
                                  <TinyVec::TinyVector<T, N>, Allocator>
                                > 
               >,
  Daixt::UnOp<ARG, RowSumOfMatrix> 
>
{
  static inline 
  typename TinyVec::TinyVector<T, N>
  Apply(const Daixt::UnOp<ARG, RowSumOfMatrix>& arg,
        std::size_t i)
  { 
    typedef typename ARG::Disambiguation::RowStorage RowStorage;
    typedef typename RowStorage::value_type::second_type BlockT;

    typedef typename RowStorage::const_iterator const_iterator;  
    
    // extract the row
    RowStorage Row =
      RowExtractor<typename ARG::Disambiguation>(i)(arg.arg());

    BlockT SumBlock = T(0);

    const_iterator end = Row.end();
    for (const_iterator iter = Row.begin(); iter != end; ++iter)
      {
        SumBlock += iter->second;
      }

    typedef typename TinyVec::TinyVector<T, N> ResultT;

    ResultT Result = 0;

    for (std::size_t i = 1; i != N+1; ++i)
      {
        for (std::size_t j = 1; j != N+1; ++j)
          {
            Result(i) += SumBlock(i, j);
          }
      }

    return Result;
  }
};



} // namespace Linalg


#endif // DAIXT_LINALG_ROW_SUM_INC
