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

#ifndef DAIXT_DEFAULT_OPS_INC
#define DAIXT_DEFAULT_OPS_INC

#include <cmath>

#include "daixtrose/Expr.h"
#include "daixtrose/UnOps.h"
#include "daixtrose/BinOps.h"
#include "daixtrose/ConstRef.h"
#include "daixtrose/CompileTimeChecks.h"

////////////////////////////////////////////////////////////////////////////////
// MACROS for the convenient creation of operations which fit as arguments
// for Daixt::Unop and Daixt::BinOp

////////////////////////////////////////////////////////////////////////////////
// A little helper to determine the return type without execution of maybe
// expensive constructors
namespace Daixt { template <class T> struct Hint {}; }

////////////////////////////////////////////////////////////////////////////////
// Macros for UnOps

#define DAIXT_DEFINE_UNARY_OPERATION(OP_NAME, DEFAULT_OP)                      \
struct OP_NAME                                                                \
{                                                                             \
  static const char* Symbol() { return #DEFAULT_OP; }                         \
                                                                              \
  template <class ARG, class ReturnType>                                      \
  static inline                                                               \
  ReturnType                                                                  \
  Apply(const ARG& arg, const Daixt::Hint<ReturnType>& Dummy)                  \
  {                                                                           \
    return DEFAULT_OP(arg);                                                   \
  }                                                                           \
}
                                                                              
#define DAIXT_DEFINE_UNARY_FUNCTION(FN_NAME, OP_NAME, DEFAULT_OP)              \
template<class ARG>                                                           \
inline                                                                        \
Daixt::Expr<                                                                   \
 Daixt::UnOp<typename                                                          \
  Daixt::UnwrapExpr<typename                                                   \
    Daixt::CRefOrVal<ARG>::Type>::Type,                                        \
  OP_NAME > >                                                                 \
FN_NAME(const ARG& arg)                                                       \
{                                                                             \
  typedef typename                                                            \
    Daixt::UnwrapExpr<typename                                                 \
      Daixt::CRefOrVal<ARG>::Type>::Type CR;                                   \
  typedef Daixt::UnOp<CR, OP_NAME> UO;                                         \
  typedef typename Daixt::Expr<UO> ReturnType;                                 \
  return ReturnType(UO(CR(Daixt::unwrap_expr(arg))));                          \
}  

                                                                              
#define DAIXT_DEFINE_UNOP_AND_FUNCTION(FN_NAME, OP_NAME, DEFAULT_OP)           \
DAIXT_DEFINE_UNARY_OPERATION(OP_NAME, DEFAULT_OP);                             \
DAIXT_DEFINE_UNARY_FUNCTION(FN_NAME, OP_NAME, DEFAULT_OP)


////////////////////////////////////////////////////////////////////////////////
// Macros for BinOps

#define DAIXT_DEFINE_BINARY_OPERATION(OP_NAME, DEFAULT_OP)                     \
struct OP_NAME                                                                \
{                                                                             \
  static const char* Symbol() { return #DEFAULT_OP; }                         \
                                                                              \
  template <class LHS, class RHS, class ReturnType>                           \
  static inline                                                               \
  ReturnType                                                                  \
  Apply(const LHS& lhs, const RHS& rhs, const Daixt::Hint<ReturnType>& Dummy)  \
  {                                                                           \
    return ReturnType(lhs DEFAULT_OP rhs);                                    \
  }                                                                           \
}

#define DAIXT_DEFINE_BINARY_FUNCTION(FN_NAME, OP_NAME, DEFAULT_OP)             \
template <class LHS, class RHS>                                               \
inline                                                                        \
Daixt::Expr<                                                                   \
  Daixt::BinOp<                                                                \
              typename Daixt::UnwrapExpr<typename CRefOrVal<LHS>::Type>::Type, \
              typename Daixt::UnwrapExpr<typename CRefOrVal<RHS>::Type>::Type, \
              OP_NAME                                                         \
              >                                                               \
           >                                                                  \
FN_NAME(const LHS& lhs, const RHS& rhs)                                       \
{                                                                             \
  typedef typename Daixt::UnwrapExpr<typename CRefOrVal<LHS>::Type>::Type LHT; \
  typedef typename Daixt::UnwrapExpr<typename CRefOrVal<RHS>::Type>::Type RHT; \
  typedef Daixt::BinOp<LHT, RHT, OP_NAME> BO;                                  \
  typedef typename Daixt::Expr<BO> ReturnType;                                 \
                                                                              \
  return ReturnType(BO(LHT(Daixt::unwrap_expr(lhs)),                           \
                       RHT(Daixt::unwrap_expr(rhs))));                         \
} 


#define DAIXT_DEFINE_BINOP_AND_FUNCTION(FN_NAME, OP_NAME, DEFAULT_OP)          \
DAIXT_DEFINE_BINARY_OPERATION(OP_NAME, DEFAULT_OP);                            \
DAIXT_DEFINE_BINARY_FUNCTION(FN_NAME, OP_NAME, DEFAULT_OP)


////////////////////////////////////////////////////////////////////////////////
// the Daixt default operations
////////////////////////////////////////////////////////////////////////////////

// the default operations (for +,-,*,/ etc.) are "hidden" in their own 
// namespace. 
// User who like to use these type a "using namespace Daixt::DefaultOps;"
// or maybe something like "using Daixt::DefaultOps::operator+;",
// users who do not like the defaults may create their own ops and use them 
// without pain.

// FIXIT: CAVEATS: If You use temporaries in an expression, like e.g. in "Matrix
// M = MakeMatrix() + MakeMatrix();" this error is not detected, since the args
// are const references.  Since the operator returns after construction of BinOp
// and UnOp (and _before_ any assignment takes place) one may end with undefined
// behaviour. One really should catch those temporaries ...

// Andrei Alexandrescou's moving objects may help here, but there are still some
// open questions (for me) with those. So <YOUR CONTRIBUTION> here.
// In the meantime I don't care. :-)
// This issue is orthogonal to daixt anyway, so a solution is easily plugged
// here later.


namespace Daixt 
{
namespace DefaultOps
{

////////////////////////////////////////////////////////////////////////////////
// UnOps
////////////////////////////////////////////////////////////////////////////////

DAIXT_DEFINE_UNOP_AND_FUNCTION(operator++, UnaryPreIncrement, ++)
DAIXT_DEFINE_UNOP_AND_FUNCTION(operator--, UnaryPreDecrement, --)

// Post*crement is a memfun, therefore missing here

DAIXT_DEFINE_UNOP_AND_FUNCTION(operator~ , UnaryComplement, ~)
DAIXT_DEFINE_UNOP_AND_FUNCTION(operator!,  UnaryNot, !)
DAIXT_DEFINE_UNOP_AND_FUNCTION(operator+ , UnaryPlus, +)
DAIXT_DEFINE_UNOP_AND_FUNCTION(operator- , UnaryMinus, -)

// To overload Adress of an expression and similar ops may be dangerous.
// Until I have a better idea I am paranoid here:
namespace Dangerous
{ 
DAIXT_DEFINE_UNOP_AND_FUNCTION(operator& , UnaryAdressOf, &)
DAIXT_DEFINE_UNOP_AND_FUNCTION(operator* , UnaryDereference, *)
}

////////////////////////////////////////////////////////////////////////////
// Rational Power
// usage: 
// const int m = 5;
// const int n = -3;
// Daixt::DefaultOps::Pow<m, n>(Something);

template<int m, int n> 
struct RationalPower 
{
public:                                                               
  template <class ARG, class ReturnType>                              
  static inline                                                       
  ReturnType                                                          
  Apply(const ARG& arg, const Hint<ReturnType>& Dummy)                
  {                                                                   
    return pow(pow(arg, m) -n);                       
  }                                                 
};   

// x^m
template<int m> 
struct RationalPower<m, 1> 
{
public:                                                               
  template <class ARG, class ReturnType>                              
  static inline                                                       
  ReturnType                                                          
  Apply(const ARG& arg, const Hint<ReturnType>& Dummy)                
  {                                               
    return pow(arg, m);
  }                                                 
};   



template<int m, int n, class ARG>
inline
Daixt::Expr<
 Daixt::UnOp<typename
  Daixt::UnwrapExpr<typename
    Daixt::CRefOrVal<ARG>::Type>::Type,
  Daixt::DefaultOps::RationalPower<m, n> > >
RationalPow(const ARG& arg) 
{
  COMPILE_TIME_ASSERT(n != 0);

  typedef typename Daixt::UnwrapExpr<typename CRefOrVal<ARG>::Type>::Type CR;
  typedef Daixt::UnOp<CR, Daixt::DefaultOps::RationalPower<m, n> > UO;
  typedef typename Daixt::Expr<UO> ReturnType;

  return ReturnType(UO(CR(unwrap_expr(arg))));
}

template<int m, class ARG>
inline 
Daixt::Expr<
 Daixt::UnOp<typename
  Daixt::UnwrapExpr<typename
    Daixt::CRefOrVal<ARG>::Type>::Type,
  Daixt::DefaultOps::RationalPower<m, 1> > >
Pow(const ARG& arg) 
{
  typedef typename Daixt::UnwrapExpr<typename CRefOrVal<ARG>::Type>::Type CR;
  typedef Daixt::UnOp<CR, Daixt::DefaultOps::RationalPower<m, 1> > UO;
  typedef typename Daixt::Expr<UO> ReturnType;

  return ReturnType(UO(CR(unwrap_expr(arg))));
}


////////////////////////////////////////////////////////////////////////////

// needed to hide it from CPP
#define RAT_POW(M, N) Daixt::DefaultOps::RationalPower<M, N>

// square root via RationalPower
DAIXT_DEFINE_UNARY_FUNCTION(Sqrt, RAT_POW(1, 2), sqrt)

// Inverse via RationalPower (I am still unhappy with the default, You may
// propose a better solution here)
DAIXT_DEFINE_UNARY_FUNCTION(Inverse, RAT_POW(1, -1), 1.0 /)

#undef RAT_POW


////////////////////////////////////////////////////////////////////////////////
// BinOps
////////////////////////////////////////////////////////////////////////////////

DAIXT_DEFINE_BINOP_AND_FUNCTION(operator* , BinaryMultiply, *)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator/ , BinaryDivide, /)

  // FIXIT: this should be added in definition: disallow division by zero
//   COMPILE_TIME_ASSERT(!(SAME_TYPE                                             
//                         (RHS, Daixt::IsNull<typename RHS::Disambiguation>)     
//                         &&                                                    
//                         SAME_TYPE(OP_NAME, Daixt::DefaultOps::BinaryDivide))); 


DAIXT_DEFINE_BINOP_AND_FUNCTION(operator% , BinaryModuloAliasRemainder, %)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator+ , BinaryPlus, +)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator- , BinaryMinus, -)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator<<, BinaryShiftLeft, <<)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator>>, BinaryShiftRight, >>)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator< , BinaryLessThan, <)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator<=, BinaryLessThanOrEqual, <=)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator> , BinaryGreaterThan, >)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator>=, BinaryGreaterThanOrEqual, >=)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator==, BinaryEqual, ==)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator!=, BinaryNotEqual, !=)


DAIXT_DEFINE_BINOP_AND_FUNCTION(operator%=, BinaryModuloAndAssign, %=)


namespace LogicalOps
{

DAIXT_DEFINE_BINOP_AND_FUNCTION(operator& , BinaryBitwiseAnd, &)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator^ , BinaryBitwiseExclusiveOr, ^)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator| , BinaryBitwiseInclusiveOr, |)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator&&, BinaryLogicalAnd, &&)
DAIXT_DEFINE_BINOP_AND_FUNCTION(operator||, BinaryLogicalInclusiveOr, ||)

} // namespace LogicalOps


////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////


} // namespace DefaultOps
} // namespace Daixt


#endif // DAIXT_DEFAULT_OPS_INC
