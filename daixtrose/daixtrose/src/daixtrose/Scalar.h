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

#ifndef DAIXT_SCALAR_INC
#define DAIXT_SCALAR_INC

namespace Daixt 
{

////////////////////////////////////////////////////////////////////////////////
// Scalar
////////////////////////////////////////////////////////////////////////////////

// the concept of a scalar may be different for every kind of concept
// represented by Disambiguation and plugged into Daixt. Therefore Daixt::Scalar
// is parametrized with the Disambiguation, so everybody can specialize the
// behaviour of a scalar. The default behaviour is to use a double.

template<class D> 
class Scalar 
{
public:
  typedef D Disambiguation;
  typedef double NumericalType;

  inline Scalar(const NumericalType& Value = NumericalType()) : Value_(Value) {}
  inline operator const NumericalType&() const { return Value_; }
  inline NumericalType Value() const { return Value_; }

private:
  NumericalType Value_;
};


namespace Convenience
{

////////////////////////////////////////////////////////////////////////////////
// for convenience: when multiplying a Daixt::Expr<T> with a double the double
// gets converted to a scalar with appropriate disambiguation. Since this might
// be a bad idea in a certain (e.g. paranoid) context, users _must_ say so:
//
// using namespace Daixt::Convenience

#define DAIXT_DEFINE_CONVENIENCE_BINOP(FN_NAME, OP_NAME)                       \
template<class ARG>                                                           \
inline                                                                        \
Daixt::Expr<                                                                   \
  Daixt::BinOp<typename                                                        \
    Daixt::UnwrapExpr<typename Daixt::CRefOrVal<ARG>::Type>::Type,              \
    Daixt::Scalar<typename ARG::Disambiguation>,                               \
    OP_NAME > >                                                               \
FN_NAME(const ARG& arg, double d)                                             \
{                                                                             \
  typedef typename                                                            \
    Daixt::UnwrapExpr<typename                                                 \
      Daixt::CRefOrVal<ARG>::Type>::Type CR;                                   \
  typedef typename ARG::Disambiguation D;                                     \
  typedef Daixt::BinOp<CR, Daixt::Scalar<D>, OP_NAME> BO;                       \
  typedef typename Daixt::Expr<BO> ReturnType;                                 \
  return ReturnType(BO(CR(Daixt::unwrap_expr(arg)), Daixt::Scalar<D>(d)));      \
}                                                                             \
                                                                              \
template<class ARG>                                                           \
inline                                                                        \
Daixt::Expr<                                                                   \
  Daixt::BinOp<                                                                \
    Daixt::Scalar<typename ARG::Disambiguation>,                               \
    typename Daixt::UnwrapExpr<typename Daixt::CRefOrVal<ARG>::Type>::Type,     \
    OP_NAME > >                                                               \
FN_NAME(double d, const ARG& arg)                                             \
{                                                                             \
  typedef typename                                                            \
    Daixt::UnwrapExpr<typename                                                 \
      Daixt::CRefOrVal<ARG>::Type>::Type CR;                                   \
  typedef typename ARG::Disambiguation D;                                     \
  typedef Daixt::BinOp<Daixt::Scalar<D>, CR, OP_NAME> BO;                       \
  typedef typename Daixt::Expr<BO> ReturnType;                                 \
  return ReturnType(BO(Daixt::Scalar<D>(d), CR(Daixt::unwrap_expr(arg))));      \
}  



DAIXT_DEFINE_CONVENIENCE_BINOP(operator* , Daixt::DefaultOps::BinaryMultiply)
DAIXT_DEFINE_CONVENIENCE_BINOP(operator/ , Daixt::DefaultOps::BinaryDivide  )
DAIXT_DEFINE_CONVENIENCE_BINOP(operator+ , Daixt::DefaultOps::BinaryPlus    )
DAIXT_DEFINE_CONVENIENCE_BINOP(operator- , Daixt::DefaultOps::BinaryMinus   )
  


} // namespace Convenience

} // namespace Daixt 



#endif // DAIXT_SCALAR_INC
