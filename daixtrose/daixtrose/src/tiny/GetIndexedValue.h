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

#ifndef TINY_GET_INDEXED_VALUE_INC
#define TINY_GET_INDEXED_VALUE_INC

#include <cstddef>

#include "loki/TypeManip.h"

#include "tiny/TinyMatrix.h"
#include "tiny/MatrixVectorOps.h"

#include "daixtrose/Daixt.h"


namespace TinyMat
{

// used for Implementation; enables user extensibility via specialization
template<class T, class ARG> struct OperatorDelimImpl;

template<class T>
class GetValue
{
private:
  size_t i_, j_;
  
public:
  inline GetValue(size_t i, size_t j) : i_(i), j_(j) {}
  
  ////////////////////////////////////////////////////////////////////////////
  // operator() forwards to user-extensible implementation
  template<class ARG> inline T operator()(const ARG& arg) const
  {
    // check disambiguation
    typedef typename Daixt::UnwrapExpr<ARG>::Type UnwrappedARG;
    typedef typename UnwrappedARG::Disambiguation Disambiguation;
    
    static const int Dimension = Disambiguation::Dimension;

    typedef 
      TinyMat::TinyQuadraticMatrixExpression<T, Dimension> 
      MyOwnDisambiguation;
    
    COMPILE_TIME_ASSERT(SAME_TYPE(Disambiguation, MyOwnDisambiguation));
    
    // perform the request
    return 
      OperatorDelimImpl<GetValue<T>, UnwrappedARG>::
      Apply(Daixt::unwrap_expr(arg), i_, j_);
  }
};


////////////////////////////////////////////////////////////////////////////////
//**************************** Implementations *******************************//
////////////////////////////////////////////////////////////////////////////////


// TinyMatrix
template <class T, int n>
struct OperatorDelimImpl<GetValue<T>, 
                         TinyQuadraticMatrix<T, n> >
{
  static inline T Apply(const TinyQuadraticMatrix<T, n>& M, size_t i, size_t j) 
  {
    return M(i, j);
  }
};


// Daixt::Expr
template <class T, class C>
struct OperatorDelimImpl<GetValue<T>, Daixt::Expr<C> > 
{
  typedef Daixt::Expr<C> ArgT;
  static inline T Apply(const ArgT& E, size_t i, size_t j) 
  {
    return GetValue<T>(i, j)(E.content());
  }
};


// Daixt::ConstRef
template <class T, int n>
struct OperatorDelimImpl<GetValue<T>, 
                         Daixt::ConstRef<TinyQuadraticMatrix<T, n> > >
{
  typedef Daixt::ConstRef<TinyQuadraticMatrix<T, n> > ArgT;
  static inline T Apply(const ArgT& M, size_t i, size_t j) 
  {
    return static_cast<const TinyQuadraticMatrix<T, n>&>(M)(i, j);
  }
};



// Daixt::Scalar Value
template <class T, int n>
struct OperatorDelimImpl<GetValue<T>,
                         Daixt::Scalar<TinyQuadraticMatrixExpression<T, n> > >
{
  typedef typename Daixt::Scalar<TinyQuadraticMatrixExpression<T, n> > ArgT;
  
  static inline T Apply(const ArgT& S, size_t i, size_t j) 
  {
    return static_cast<T>(S);
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: apply default behaviour of unary operation to all entries.
// this means e.g.  A = sqrt(B)  yields  A(i,j) = sqrt(B(i,j))

template<class T, class ARG, class Op>
struct 
OperatorDelimImpl<GetValue<T>, Daixt::UnOp<ARG, Op> >
{
  static inline T Apply(const Daixt::UnOp<ARG, Op>& UO, size_t i, size_t j) 
  {
    return Op::Apply(GetValue<T>(i, j)(UO.arg()), 
                     Daixt::Hint<T>());
  }
};


// transposed matrix
template<class T, class ARG>
struct 
OperatorDelimImpl<GetValue<T>, 
                  Daixt::UnOp<ARG, TinyMat::TransposeOfTinyMatrix> >
{
  static inline T Apply(const Daixt::UnOp<ARG, TinyMat::TransposeOfTinyMatrix>& UO, 
                        size_t i, size_t j) 
  {
    return GetValue<T>(j, i)(UO.arg());
  }
};


// lumped matrix
template<class T, class ARG>
struct 
OperatorDelimImpl<GetValue<T>, 
                  Daixt::UnOp<ARG, TinyMat::LumpedTinyMatrix> >
{
  static inline T Apply(const Daixt::UnOp<ARG, TinyMat::LumpedTinyMatrix>& UO, 
                        size_t i, size_t j) 
  {
    if (i != j) return T();

    typedef typename ARG::Disambiguation Disambiguation;
    size_t k_max = Disambiguation::Dimension + 1;

    T Result = T();
    
    for (size_t k = 1; k != k_max; ++k)
      { 
        Result += GetValue<T>(i, k)(UO.arg());
      }
    
    return Result; // row sum.
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOps

// BinaryPlus
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<GetValue<T>, 
                  Daixt::BinOp<LHS, RHS, 
                              Daixt::DefaultOps::BinaryPlus> >
{
  COMPILE_TIME_ASSERT(LHS::Disambiguation::Dimension 
                      == 
                      RHS::Disambiguation::Dimension);

  static inline T Apply(const Daixt::BinOp<LHS, RHS, 
                                          Daixt::DefaultOps::BinaryPlus>& BO, 
                        size_t i, size_t j)
  {
    return 
      GetValue<T>(i, j)(BO.lhs())
      +
      GetValue<T>(i, j)(BO.rhs());
  }
};


// BinaryMinus
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<GetValue<T>, 
                  Daixt::BinOp<LHS, RHS, 
                              Daixt::DefaultOps::BinaryMinus> >
{
  COMPILE_TIME_ASSERT(LHS::Disambiguation::Dimension 
                      == 
                      RHS::Disambiguation::Dimension);

  static inline T Apply(const Daixt::BinOp<LHS, RHS, 
                                          Daixt::DefaultOps::BinaryMinus>& BO, 
                        size_t i, size_t j)
  {
    return 
      GetValue<T>(i, j)(BO.lhs())
      -
      GetValue<T>(i, j)(BO.rhs());
  }
};


// BinaryMultiply
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<GetValue<T>, 
                  Daixt::BinOp<LHS, RHS, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  COMPILE_TIME_ASSERT(LHS::Disambiguation::Dimension 
                      == 
                      RHS::Disambiguation::Dimension);


  static inline T Apply
  (const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply>& BO, 
   size_t i, size_t j)
  {
    T Result = T();
    
    typedef typename LHS::Disambiguation Disambiguation;
    size_t k_max = Disambiguation::Dimension + 1;

    for (size_t k = 1; k != k_max; ++k)
      { 
        Result += 
          (
           GetValue<T>(i, k)(BO.lhs())
           *
           GetValue<T>(k, j)(BO.rhs())
           );
      }

    return Result;
  }
};


// TinyMatrix * Scalar
template<class T, class LHS, class D>
struct
OperatorDelimImpl<GetValue<T>, 
                  Daixt::BinOp<LHS, 
                              Daixt::Scalar<D>, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline T Apply
  (const Daixt::BinOp<LHS, Daixt::Scalar<D>, Daixt::DefaultOps::BinaryMultiply>& BO, 
   size_t i, size_t j)
  {
    return GetValue<T>(i, j)(BO.lhs()) * BO.rhs().Value();
  }
};


// Scalar * TinyMatrix
template<class T, class D, class RHS>
struct
OperatorDelimImpl<GetValue<T>, 
                  Daixt::BinOp<Daixt::Scalar<D>, 
                              RHS, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline T Apply
  (const Daixt::BinOp<Daixt::Scalar<D>, RHS, Daixt::DefaultOps::BinaryMultiply>& BO, 
   size_t i, size_t j)
  {
    return GetValue<T>(i, j)(BO.rhs()) * BO.lhs().Value();
  }
};

// FIXIT: Many more specializations missing 
  

} // namespace TinyMat


////////////////////////////////////////////////////////////////////////////////
//************************** TinyVector **************************************//
////////////////////////////////////////////////////////////////////////////////


namespace TinyVec
{

// used for Implementation; enables user extensibility via specialization
template<class T, class ARG> struct OperatorDelimImpl;

template<class T>
class GetValue
{
private:
  size_t j_;
  
public:
  inline GetValue(size_t j) : j_(j) {}
  
  ////////////////////////////////////////////////////////////////////////////
  // operator() forwards to user-extensible implementation
  template<class ARG> inline T operator()(const ARG& arg) const
  {
    // check disambiguation
    typedef typename ARG::Disambiguation Disambiguation;
    typedef 
      TinyVec::TinyVectorExpression<T, Disambiguation::Dimension> 
      MyOwnDisambiguation;
    
    COMPILE_TIME_ASSERT(SAME_TYPE(Disambiguation, MyOwnDisambiguation));
    
    // perform the request
    return 
      OperatorDelimImpl<GetValue<T>, ARG>::Apply(arg, j_);
  }
};


////////////////////////////////////////////////////////////////////////////////
//**************************** Implementations *******************************//
////////////////////////////////////////////////////////////////////////////////


// TinyVector
template <class T, int n>
struct OperatorDelimImpl<GetValue<T>, 
                         TinyVector<T, n> >
{
  static inline T Apply(const TinyVector<T, n>& V, size_t j) 
  {
    return V(j);
  }
};


// Daixt::Expr
template <class T, class C>
struct OperatorDelimImpl<GetValue<T>, Daixt::Expr<C> > 
{
  typedef Daixt::Expr<C> ArgT;
  static inline T Apply(const ArgT& E, size_t j) 
  {
    return GetValue<T>(j)(E.content());
  }
};


// Daixt::ConstRef
template <class T, int n>
struct OperatorDelimImpl<GetValue<T>, 
                         Daixt::ConstRef<TinyVector<T, n> > >
{
  typedef Daixt::ConstRef<TinyVector<T, n> > ArgT;
  static inline T Apply(const ArgT& V, size_t j) 
  {
    return static_cast<const TinyVector<T, n>&>(V)(j);
  }
};



// Daixt::Scalar Value
template <class T, int n>
struct OperatorDelimImpl<GetValue<T>,
                         Daixt::Scalar<TinyVectorExpression<T, n> > >
{
  typedef typename Daixt::Scalar<TinyVectorExpression<T, n> > ArgT;
  
  static inline T Apply(const ArgT& S, size_t j) 
  {
    return static_cast<T>(S);
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: apply default behaviour of unary operation element-wise to all
// entries.  this means e.g.  A = sqrt(B) yields A(j) = sqrt(B(j)).

template<class T, class ARG, class Op>
struct 
OperatorDelimImpl<GetValue<T>, Daixt::UnOp<ARG, Op> >
{
  static inline T Apply(const Daixt::UnOp<ARG, Op>& UO, size_t j) 
  {
    return Op::Apply(GetValue<T>(j)(UO.arg()), 
                     Daixt::Hint<T>());
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOps (OP is applied member wise, so vector * vector is still a vector)

template<class T, class LHS, class RHS, class OP>
struct
OperatorDelimImpl<GetValue<T>, Daixt::BinOp<LHS, RHS, OP> >
{
  static const size_t N1 = LHS::Disambiguation::Dimension; 
  static const size_t N2 = RHS::Disambiguation::Dimension; 

  COMPILE_TIME_ASSERT(N1 == N2);


  static inline T Apply(const Daixt::BinOp<LHS, RHS, OP>& BO, 
                        size_t j)
  {
    return Apply(BO, j, Overload());
  }

private:
  // check whether we deal with a matrix times vector case here
  typedef TinyMat::TinyQuadraticMatrixExpression<T, N1> TQM;
  typedef TinyVec::TinyVectorExpression<T, N2> TV;

  static const bool MatrixTimesVector =
  (SAME_TYPE(typename LHS::Disambiguation, TQM)
   &&
   SAME_TYPE(OP, Daixt::DefaultOps::BinaryMultiply)
   &&
   SAME_TYPE(typename RHS::Disambiguation, TV));

  
  template <int i> struct Overloader {};

  typedef typename Loki::Select<MatrixTimesVector,
                                Overloader<1>,
                                Overloader<0> >::Result Overload;
  
  //////////////////////////////////////////////////////////////////////////////
  // the normal forward to OP
  static inline T Apply(const Daixt::BinOp<LHS, RHS, OP>& BO, 
                        size_t j, 
                        const Overloader<0>& Ignored)
  {
    return 
      OP::Apply(GetValue<T>(j)(BO.lhs()),
                GetValue<T>(j)(BO.rhs()),
                Daixt::Hint<T>());
  }

  //////////////////////////////////////////////////////////////////////////////
  // matrix-expression times vector-expression needs extra work: here we try
  // imperfect loop unrolling
  template 
  <size_t N, 
   size_t FuckTheStandardWhichDoesNotAllowExplicitTemplSpecInsideClass = 0>
  class MatVecMult
  {
  private:
    size_t RowNumber_;
  public:
    inline MatVecMult(size_t RowNumber) : RowNumber_(RowNumber) {}
    inline T Result (const LHS& lhs, const RHS& rhs)
    {
      // MatDataBegin: TinyMatrix has Fortran storage 
      return 
        TinyMat::GetValue<T>(RowNumber_, N)(lhs)
        *
        TinyVec::GetValue<T>(N)(rhs)
        +
        MatVecMult<N-1>(RowNumber_).Result(lhs, rhs);
    }
  };
  
  template <size_t SeeAbove>
  class MatVecMult<1, SeeAbove>
  {
  private:
    size_t RowNumber_;
  public:
    inline MatVecMult(size_t RowNumber) : RowNumber_(RowNumber) {}
    inline T Result (const LHS& lhs, const RHS& rhs)
    {
      return 
        TinyMat::GetValue<T>(RowNumber_, 1)(lhs)
        *
        TinyVec::GetValue<T>(1)(rhs);
    }
  };
  

  ////////////////////////////////////////////////////////////////////////////////
  static inline T Apply(const Daixt::BinOp<LHS, RHS, OP>& BO, 
                        size_t j, 
                        const Overloader<1>& Ignored)
  {
    return MatVecMult<N2>(j).Result(BO.lhs(), BO.rhs());
  }

};



} // namespace TinyVec





#endif // TINYMAT_GET_INDEXED_VALUE_INC
