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

#ifndef DAIXT_LINALG_LUMP_INC
#define DAIXT_LINALG_LUMP_INC


#include "linalg/RowAndColumCounters.h"
#include "linalg/RowAndColumExtractors.h"
#include "linalg/Disambiguation.h"
#include "linalg/Matrix.h"


namespace Linalg
{
#define TINYMAT_EMPTY_ARG DoNothing

DAIXT_DEFINE_UNOP_AND_FUNCTION(Lump, LumpedMatrix, TINYMAT_EMPTY_ARG)

// Row and operators return the column operators and vice versa


template<class T, class ARG>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, LumpedMatrix> >
{
  static inline 
  typename T::RowStorage
  Apply(const Daixt::UnOp<ARG, LumpedMatrix>& arg,
        std::size_t i)
  { 
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::value_type Pair;  
    typedef typename RowStorage::const_iterator const_iterator;  
    typedef typename T::NumT NumT;

    // extract the row
    RowStorage Result =
      RowExtractor<MatrixExpression<T> >(i)(arg.arg());

 
    if (Result.size())
      {
        // accumulate all its values
        NumT Value = NumT(0);
        
        const_iterator end = Result.end();
        for (const_iterator iter = Result.begin(); iter != end; ++iter)
          {
            Value += iter->second;
          }

        // put it back into the right place 
        Result.clear();
        Result[i] = Value;
      }

    return Result;
  }
};
  

template<class T, class ARG>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, LumpedMatrix> >
{
  static inline 
  typename T::RowStorage
  Apply(const Daixt::UnOp<ARG, LumpedMatrix>& arg,
        std::size_t i)
  { 
    // It's so Easy! Lumped Matrix has only entries on diagonal.
    // => Delegate to RowExtractor
    return RowExtractor<MatrixExpression<T> >(i)(arg);
  }
};


} // namespace Linalg



#endif // DAIXT_LINALG_LUMP_INC
