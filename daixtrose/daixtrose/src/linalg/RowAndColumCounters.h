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

#ifndef DAIXT_LINALG_ROW_AND_COLUMN_COUNTERS_INC
#define DAIXT_LINALG_ROW_AND_COLUMN_COUNTERS_INC

#include "daixtrose/Daixt.h"

#include "linalg/Disambiguation.h"

#include <stdexcept>

namespace Linalg
{

template<class Disambiguation> class RowCounter;
template<class Disambiguation> class ColumnCounter;


////////////////////////////////////////////////////////////////////////////////
// user's interface
////////////////////////////////////////////////////////////////////////////////

template <class T>
inline size_t NumberOfRows(const T& t)
{
  // Splitting the matching mechanism into 2 steps: We first dispatch
  // with regard to the disambiguation and then "overload" with regard to the
  // argument. This protects You from many surprises. 
  return RowCounter<typename T::Disambiguation>()(t);
}

template <class T>
inline size_t NumberOfCols(const T& t)
{
  // Comment analogue to above.
  return ColumnCounter<typename T::Disambiguation>()(t);
}

// Overloading is implemented by forwarding all calls to a yet-to-be-specialized
// template class OperatorDelimImpl.  Using this extra indirection avoids any
// brain damage due to lookup rules, function template overload, ADL, etc. If
// You need extra functionality, write it and include it.  All visible
// specializations of OperatorDelimImpl will be considered by the compiler.
template<class T, class ARG> struct OperatorDelimImpl;


////////////////////////////////////////////////////////////////////////////////
// row counter for matrix expression
////////////////////////////////////////////////////////////////////////////////

template<class T>
struct RowCounter<MatrixExpression<T> >
{
  template<class ARG> inline size_t operator()(const ARG& arg) const
  {
    return 
      OperatorDelimImpl<
                        RowCounter<MatrixExpression<T> >, 
                        typename Daixt::UnwrapExpr<ARG>::Type
                        >::Apply(Daixt::unwrap_expr(arg));
  }
};


////////////////////////////////////////////////////////////////////////////////
// Matrix
template <class T, class RowStorage, class Allocator> class Matrix;

template<class T>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Linalg::Matrix<typename T::NumT, 
                                 typename T::RowStorage,
                                 typename T::Allocator> >
{
  static inline size_t Apply(const Linalg::Matrix<typename T::NumT, 
                                                  typename T::RowStorage,
                                                  typename T::Allocator>& arg) 
  {
    return arg.nrows();
  }
};


////////////////////////////////////////////////////////////////////////////////
// Daixt::ConstRef
template <class T, class Arg>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Daixt::ConstRef<Arg> >
{
  static inline
  size_t
  Apply(const Daixt::ConstRef<Arg>& arg)
  {
    return RowCounter<MatrixExpression<T> >()(static_cast<const Arg&>(arg));
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: assume the UnOp has no effect on the shape
template<class T, class ARG, class Op>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, Op> >
{
  static inline
  size_t Apply(const Daixt::UnOp<ARG, Op>& arg)
  { 
    return RowCounter<MatrixExpression<T> >()(arg.arg());
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOps

// no default, if we use crazy OPs, we need to add further specializations

// matrix expression + matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus>& arg)
  {
    const size_t LHS_Result = RowCounter<MatrixExpression<T> >()(arg.lhs());
    const size_t RHS_Result = RowCounter<MatrixExpression<T> >()(arg.rhs());
    
    if (LHS_Result != RHS_Result) 
      throw std::invalid_argument
        ("Incompatible matrices in expression: number of rows do not match");

    return LHS_Result;
  }
};

// matrix expression - matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus>& arg)
  {
    const size_t LHS_Result = RowCounter<MatrixExpression<T> >()(arg.lhs());
    const size_t RHS_Result = RowCounter<MatrixExpression<T> >()(arg.rhs());
    
    if (LHS_Result != RHS_Result) 
      throw std::invalid_argument
        ("Incompatible matrices in expression: number of rows do not match");

    return LHS_Result;
  }
};


// matrix expression * matrix expression -> lhs
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> >
{
  static inline size_t Apply
  (const Daixt::BinOp<LHS, RHS, 
                     Daixt::DefaultOps::BinaryMultiply>& arg)
  {
    return RowCounter<MatrixExpression<T> >()(arg.lhs());
  }
};


// scalar value * matrix expression -> rhs
template<class T, class RHS>
struct
OperatorDelimImpl<RowCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<Daixt::Scalar<MatrixExpression<T> >, 
                              RHS, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  size_t Apply(const Daixt::BinOp<Daixt::Scalar<MatrixExpression<T> >, 
                           RHS, 
                           Daixt::DefaultOps::BinaryMultiply>& arg)
  {
    return RowCounter<MatrixExpression<T> >()(arg.rhs());
  }
};
  


////////////////////////////////////////////////////////////////////////////////
// row counter for vector expression
////////////////////////////////////////////////////////////////////////////////

template<class T> 
struct RowCounter<VectorExpression<T> >
{
  template<class ARG> inline size_t operator()(const ARG& arg) const
  {
    return 
      OperatorDelimImpl<
                        RowCounter<VectorExpression<T> >, 
                        typename Daixt::UnwrapExpr<ARG>::Type
                        >::Apply(Daixt::unwrap_expr(arg));
  }
};


// Vector
template <class T, class Allocator> class Vector;

template<class T>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Linalg::Vector<typename T::NumT, typename T::Allocator> >
{
  static inline 
  size_t Apply(const Linalg::Vector<typename T::NumT, 
                                    typename T::Allocator>& V) 
  {
    return V.size();
  }
};

// OperatorDelimImpl<RowCounter<VectorExpression<VectorDisambiguator<double,
//    AL > > >, Vector<double, AL> >::Apply'

////////////////////////////////////////////////////////////////////////////////
// Daixt::ConstRef
template <class T, class Arg>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Daixt::ConstRef<Arg> >
{
  static inline
  size_t
  Apply(const Daixt::ConstRef<Arg>& arg)
  {
    return RowCounter<VectorExpression<T> >()(static_cast<const Arg&>(arg));
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: assume the UnOp has no effect on the shape
template<class T, class ARG, class Op>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Daixt::UnOp<ARG, Op> >
{
  static inline
  size_t Apply(const Daixt::UnOp<ARG, Op>& arg)
  { 
    return RowCounter<VectorExpression<T> >()(arg.arg());
  }
};

////////////////////////////////////////////////////////////////////////////////
// BinOps

// no default, if we use crazy OPs, we need to add further specializations

// vector expression + vector expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus>& arg)
  {
    const size_t LHS_Result = RowCounter<VectorExpression<T> >()(arg.lhs());
    const size_t RHS_Result = RowCounter<VectorExpression<T> >()(arg.rhs());
    
    if (LHS_Result != RHS_Result) 
      throw std::invalid_argument
        ("Incompatible vectors in expression: number of rows do not match");

    return LHS_Result;
  }
};


// vector expression - vector expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus>& arg)
  {
    const size_t LHS_Result = RowCounter<VectorExpression<T> >()(arg.lhs());
    const size_t RHS_Result = RowCounter<VectorExpression<T> >()(arg.rhs());
    
    if (LHS_Result != RHS_Result) 
      throw std::invalid_argument
        ("Incompatible vectors in expression: number of rows do not match");

    return LHS_Result;
  }
};

// matrix expression * vector expression 
// or
// vector expression * scalar
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, 
                                 Daixt::DefaultOps::BinaryMultiply>& arg)
  {
    return RowCounter<typename LHS::Disambiguation>()(arg.lhs());
  }
};


// scalar value * vector expression -> rhs
template<class T, class RHS>
struct
OperatorDelimImpl<RowCounter<VectorExpression<T> >, 
                  Daixt::BinOp<Daixt::Scalar<VectorExpression<T> >, 
                              RHS, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  size_t Apply(const Daixt::BinOp<Daixt::Scalar<VectorExpression<T> >, 
                           RHS, 
                           Daixt::DefaultOps::BinaryMultiply>& arg)
  {
    return RowCounter<VectorExpression<T> >()(arg.rhs());
  }
};


////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// column counter for matrix expression
////////////////////////////////////////////////////////////////////////////////

template<class T>
struct ColumnCounter<MatrixExpression<T> >
{
  template<class ARG> inline size_t operator()(const ARG& arg) const
  {
    return 
      OperatorDelimImpl<
                        ColumnCounter<MatrixExpression<T> >, 
                        typename Daixt::UnwrapExpr<ARG>::Type
                        >::Apply(Daixt::unwrap_expr(arg));
  }
};


////////////////////////////////////////////////////////////////////////////////
// Matrix
template<class T>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Matrix<typename T::NumT, 
                         typename T::RowStorage,
                         typename T::Allocator> >
{
  static inline size_t Apply(const Matrix<typename T::NumT, 
                                          typename T::RowStorage,
                                          typename T::Allocator>& arg) 
  {
    return arg.ncols();
  }
};


////////////////////////////////////////////////////////////////////////////////
// Daixt::ConstRef
template <class T, class Arg>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Daixt::ConstRef<Arg> >
{
  static inline
  size_t
  Apply(const Daixt::ConstRef<Arg>& arg)
  {
    return ColumnCounter<MatrixExpression<T> >()(static_cast<const Arg&>(arg));
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: assume the UnOp has no effect on the shape
template<class T, class ARG, class Op>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, Op> >
{
  static inline
  size_t Apply(const Daixt::UnOp<ARG, Op>& arg)
  { 
    return ColumnCounter<MatrixExpression<T> >()(arg.arg());
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOps

// no default, if we use crazy OPs, we need to add further specializations

// matrix expression + matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus>& arg)
  {
    const size_t LHS_Result = ColumnCounter<MatrixExpression<T> >()(arg.lhs());
    const size_t RHS_Result = ColumnCounter<MatrixExpression<T> >()(arg.rhs());
    
    if (LHS_Result != RHS_Result) 
      throw std::invalid_argument
        ("Incompatible matrices in expression: number of columns do not match");

    return LHS_Result;
  }
};

// matrix expression - matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus>& arg)
  {
    const size_t LHS_Result = ColumnCounter<MatrixExpression<T> >()(arg.lhs());
    const size_t RHS_Result = ColumnCounter<MatrixExpression<T> >()(arg.rhs());
    
    if (LHS_Result != RHS_Result) 
      throw std::invalid_argument
        ("Incompatible matrices in expression: number of columns do not match");

    return LHS_Result;
  }
};


// matrix expression * matrix expression -> rhs
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply>& arg)
  {
    return ColumnCounter<MatrixExpression<T> >()(arg.rhs());
  }
};


// matrix expression * scalar value -> lhs
template<class T, class LHS>
struct
OperatorDelimImpl<ColumnCounter<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, 
                              Daixt::Scalar<MatrixExpression<T> >, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  size_t Apply(const Daixt::BinOp<LHS, 
                                 Daixt::Scalar<MatrixExpression<T> >, 
                                 Daixt::DefaultOps::BinaryMultiply>& arg)
  {
    return ColumnCounter<MatrixExpression<T> >()(arg.lhs());
  }
};


} // namespace Linalg

#endif // DAIXT_LINALG_ROW_AND_COLUMN_COUNTERS_INC

