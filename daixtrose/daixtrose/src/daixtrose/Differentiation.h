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
// developers of daixtrose (see e.g. http://daixtrose.sourceforge.net/)

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU Lesser General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU Lesser General Public License.

#ifndef DAIXT_DIFFERENTIATION_INC
#define DAIXT_DIFFERENTIATION_INC

#include <limits>

#include "loki/TypeManip.h"

#include "daixtrose/BinOps.h"
#include "daixtrose/UnOps.h"
#include "daixtrose/ConstRef.h"
#include "daixtrose/DefaultOps.h"
#include "daixtrose/Disambiguation.h"
#include "daixtrose/Expr.h"
#include "daixtrose/CountLeaves.h"
#include "daixtrose/CountOccurence.h"

#include "daixtrose/NeutralElements.h"
#include "daixtrose/Scalar.h"
#include "daixtrose/CompileTimeChecks.h"

#include "daixtrose/MatrixSelect.h"


////////////////////////////////////////////////////////////////////////////////
// Information and tools for compile-time differentiation of expression
// templates. This differentiaion is based on distiction by type, so it is not
// applicable to all leaf types that fit into the Daixt Expression Template
// engine.
////////////////////////////////////////////////////////////////////////////////

namespace Daixt 
{
namespace Differentiation 
{

// user extensible approach through partially specializable class  
template<class WRT, class ARG> struct DiffImpl;

////////////////////////////////////////////////////////////////////////////////
template <class WRT> // WRT = With Regard To
struct Differentiator {
  // delegate to a specializable template 
  template<class ARG> 
  inline 
  typename DiffImpl<WRT, ARG>::ReturnType
  operator()(const ARG& arg) const 
  { 
    COMPILE_TIME_ASSERT(SAME_TYPE(typename WRT::Disambiguation, 
                                  typename ARG::Disambiguation));

    return DiffImpl<WRT, ARG>::Apply(arg); 
  }
};


////////////////////////////////////////////////////////////////////////////////
// function for convenient call
template<class WRT, class ARG> 
inline 
Daixt::Expr<typename DiffImpl<WRT, ARG>::ReturnType>
Diff(const ARG& arg, const WRT& wrt)
{
  typedef Daixt::Expr<typename DiffImpl<WRT, ARG>::ReturnType> ReturnType;
  return ReturnType(Differentiator<WRT>()(arg));
}


////////////////////////////////////////////////////////////////////////////////
// implementation and some important specializations for DiffImpl


////////////////////////////////////////////////////////////////////////////////
// default: compare WRT and ARG and return 0 or 1
template<class WRT, class ARG> struct DiffImpl
{
  typedef IsOne<typename ARG::Disambiguation> One;
  typedef IsNull<typename ARG::Disambiguation> Null;
  static const bool Condition = SAME_TYPE(WRT, ARG);

  typedef typename Loki::Select<Condition, One, Null>::Result ReturnType;

  static inline ReturnType Apply(const ARG& UO)  
  {
    return ReturnType();
  }
};


////////////////////////////////////////////////////////////////////////////////
// Epr<T>: unpack and forward
template <class WRT, class T>
struct DiffImpl<WRT, Daixt::Expr<T> >
{
  typedef typename DiffImpl<WRT, T>::ReturnType ReturnType;

  static inline ReturnType Apply(const Daixt::Expr<T>& E)  
  {
    return DiffImpl<WRT, T>::Apply(E.content());
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps: default behaviour: differentiate argument and apply UnOp to the
// result.  This is OK for UnOps that are neutral with regard to differentiation
// like e.g. UnaryMinus, UnaryPlus, etc., others catched during compile
template <class WRT, class ARG, class OP>
struct DiffImpl<WRT, Daixt::UnOp<ARG, OP> >
{
  typedef typename DiffImpl<WRT, ARG>::ReturnType ArgReturnType;

  struct XXX_____________Sorry_Not_Implemented_Yet_________________XXX {};

  typedef typename
  Loki::Select<(
                SAME_TYPE(OP, Daixt::DefaultOps::UnaryPlus)
                ||
                SAME_TYPE(OP, Daixt::DefaultOps::UnaryMinus)
                ),
               Daixt::UnOp<ArgReturnType, OP>,
               XXX_____________Sorry_Not_Implemented_Yet_________________XXX
               >::Result ReturnType;


  static inline ReturnType Apply(const Daixt::UnOp<ARG, OP>& UO)  
  {
    return ReturnType(DiffImpl<WRT, ARG>::Apply(UO.arg()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOp
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// BinOps default (restricted by compile-time checks): (f OP g)' = f' OP g'
template <class WRT, class LHS, class RHS, class OP>
struct DiffImpl<WRT, Daixt::BinOp<LHS, RHS, OP> >
{
  typedef typename DiffImpl<WRT, LHS>::ReturnType DiffedLHS;
  typedef typename DiffImpl<WRT, RHS>::ReturnType DiffedRHS;

  struct XXX_____________Sorry_Not_Implemented_Yet_________________XXX {};
  
    typedef typename
    Loki::Select<(
                  SAME_TYPE(OP, Daixt::DefaultOps::BinaryPlus)
                  ||
                  SAME_TYPE(OP, Daixt::DefaultOps::BinaryMinus)
                  ),
                 Daixt::BinOp<DiffedLHS, DiffedRHS, OP>,
                 XXX_____________Sorry_Not_Implemented_Yet_________________XXX
                 >::Result ReturnType;


  
  typedef Daixt::BinOp<LHS, RHS, OP> ArgT;



  static inline ReturnType Apply(const ArgT& BO)  
  {
    
    return ReturnType(DiffImpl<WRT, LHS>::Apply(BO.lhs()),
                      DiffImpl<WRT, RHS>::Apply(BO.rhs()));
  }
};
 

////////////////////////////////////////////////////////////////////////////////
// BinaryMultiply: (f * g)' = f' * g + f * g'
template <class WRT, class LHS, class RHS>
struct DiffImpl<WRT, Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> >
{
  typedef typename DiffImpl<WRT, LHS>::ReturnType DiffedLHS;
  typedef typename DiffImpl<WRT, RHS>::ReturnType DiffedRHS;

  // f' * g
  typedef Daixt::BinOp<DiffedLHS, RHS, Daixt::DefaultOps::BinaryMultiply> Part1;
  // f * g'
  typedef Daixt::BinOp<LHS, DiffedRHS, Daixt::DefaultOps::BinaryMultiply> Part2;
  // f' * g + f * g'
  typedef Daixt::BinOp<Part1, Part2, Daixt::DefaultOps::BinaryPlus> ReturnType;
  
  typedef Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> ArgT;

  static inline ReturnType Apply(const ArgT& BO)  
  {
    
    return ReturnType(
                      Part1(DiffImpl<WRT, LHS>::Apply(BO.lhs()),
                            BO.rhs()),
                      Part2(BO.lhs(),
                            DiffImpl<WRT, RHS>::Apply(BO.rhs()))
                      );

  }
};

////////////////////////////////////////////////////////////////////////////////
// BinaryDivide: forward to BinaryMultiply: (a / b)' = (a * b^(-1))'
template <class WRT, class LHS, class RHS>
struct DiffImpl<WRT, Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryDivide> >
{
private:
  // check for division by zero                                               
  typedef Daixt::IsNull<typename RHS::Disambiguation> Null;
  COMPILE_TIME_ASSERT(!SAME_TYPE(RHS, Null));

public:
  // RHS^(-1)
  typedef Daixt::UnOp<RHS, Daixt::DefaultOps::RationalPower<-1, 1> > NewRHS;  

  // LHS * RHS^(-1)
  typedef Daixt::DefaultOps::BinaryMultiply BinaryMultiply;
  typedef Daixt::BinOp<LHS, NewRHS, BinaryMultiply> NewBinOp;

  typedef typename DiffImpl<WRT, NewBinOp>::ReturnType ReturnType;
  typedef Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryDivide> ArgT;

  static ReturnType Apply(const ArgT& BO)
  {
    return DiffImpl<WRT, NewBinOp>::Apply(NewBinOp(BO.lhs(), NewRHS(BO.rhs())));
  }
};


////////////////////////////////////////////////////////////////////////////////
// Daixt::DefaultOps::RationalPower has its own differentiation rule
// (pow(f, k))' = k * pow(f, k-1) * f'  with  k = m/n here
template <class WRT, int mm, int nn, class ARG>
struct DiffImpl<WRT, Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<mm, nn> > >
{
  typedef Daixt::Scalar<typename WRT::Disambiguation> Scalar;
  typedef typename Scalar::NumericalType NumericalType;

  // Now we go astray with our basic design where we completely separate 
  // expression simplifying from differentiation rules and simplify 
  // this expression a little bit:
  // if (nn < 0) we multiply mm and nn with (-1) in order to have some
  // nice values
  static const int m = (nn < 0) ? -mm : mm;
  static const int n = (nn < 0) ? -nn : nn;

  // we want to mutiply the result with Scalar(m)/Scalar(n), so 1/n must be possible
  COMPILE_TIME_ASSERT(!std::numeric_limits<NumericalType>::is_integer);
  // negative values must be possible
  COMPILE_TIME_ASSERT(std::numeric_limits<NumericalType>::is_signed);

  // paranoia: maybe someone calls this in a recursive way
  // not accepted by intel's C++: ((m-n) > std::numeric_limits<int>::min());
  COMPILE_TIME_ASSERT((n>0) && ((m-n) < m));
  COMPILE_TIME_ASSERT(n != 0); // paranoia

  //////////////////////////////////////////////////////////////////////////////
  // some type calculation wizardry

  typedef Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<mm, nn> > ArgT;

  //   if (m/n == 1)  ScalarFactorT is One 
  //   if (m/n == -1) ScalarFactorT is -One 
  //   if (m == 1)    ScalarFactorT is Scalar(n)^(-1)
  //   if (m != 1) but (m < 0) ScalarFactorT is  - Scalar(-m)
  //   else ScalarFactorT = Scalar(m/n)

  typedef IsNull<typename ArgT::Disambiguation> Null;
  typedef IsOne<typename ArgT::Disambiguation> One;
  typedef Daixt::UnOp<Scalar, Daixt::DefaultOps::RationalPower<-1, 1> > InverseOfN;
  typedef Daixt::UnOp<Scalar, Daixt::DefaultOps::UnaryMinus > MinusScalar;
  typedef Daixt::UnOp<One, Daixt::DefaultOps::UnaryMinus> MinusOne;

  // If You understand that one You are really good ;-)
  typedef typename Loki::Select<
    ((m == n) || (m == -n)),
    typename Loki::Select<(m == n), 
                          One, 
                          MinusOne
                          >::Result,
    typename Loki::Select<(m == 1), 
                          InverseOfN, 
                          typename Loki::Select<(m < 0),
                                                MinusScalar,
                                                Scalar
                                                >::Result
                          >::Result
  >::Result ScalarFactorT;


  // type of pow(arg, ((m-n)/n))
  typedef Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<m-n, n> > ModPowArgT;

  //  Scalar(m)[/Scalar(n)] * pow(arg, ((m-n)/n)) 

  // the type of the whole factor is 
  typedef Daixt::BinOp<ScalarFactorT, ModPowArgT, 
                      Daixt::DefaultOps::BinaryMultiply> FactorT;


  //////////////////////////////////////////////////////////////////////////////
  typedef Daixt::BinOp<FactorT, 
                      typename DiffImpl<WRT, ARG>::ReturnType, 
                      Daixt::DefaultOps::BinaryMultiply> ReturnType;


  static inline ReturnType Apply(const ArgT& UO)  
  {
    return Apply(UO, Overload());
  }

private:
  template <int i> struct Overloader {};

  typedef typename Loki::Select<
    ((m == n) || (m == -n)),
    typename Loki::Select<(m == n), 
                          Overloader<0>, // One, 
                          Overloader<1>  // MinusOne
                         >::Result,
    typename Loki::Select<(m == 1), 
                          Overloader<2>, // InverseOfN = 1/n, 
                          typename Loki::Select<(m < 0),
                                                Overloader<3>, // - (-m/n),
                                                Overloader<4>  // m/n
                                                >::Result
                          >::Result
  >::Result Overload;


  static inline ReturnType Apply(const ArgT& UO, const Overloader<0>& Ignored)  
  {
    return ReturnType(
                      FactorT(
                              One(),
                              ModPowArgT(UO.arg())
                              ),
                      DiffImpl<WRT, ARG>::Apply(UO.arg())
                      );
  }  


  static inline ReturnType Apply(const ArgT& UO, const Overloader<1>& Ignored)  
  {
    return ReturnType(
                      FactorT(
                              MinusOne(One()),
                              ModPowArgT(UO.arg())
                              ),
                      DiffImpl<WRT, ARG>::Apply(UO.arg())
                      );
  }  


  static inline ReturnType Apply(const ArgT& UO, const Overloader<2>& Ignored)  
  {
    return ReturnType(
                      FactorT(
                              InverseOfN(Scalar(static_cast<NumericalType>(n))),
                              ModPowArgT(UO.arg())
                              ),
                      DiffImpl<WRT, ARG>::Apply(UO.arg())
                      );
  }


  static inline ReturnType Apply(const ArgT& UO, const Overloader<3>& Ignored)  
  {
    return ReturnType(
                      FactorT(
                              MinusScalar(Scalar(static_cast<NumericalType>(-m)
                                                 /
                                                 static_cast<NumericalType>(n))),
                              ModPowArgT(UO.arg())
                              ),
                      DiffImpl<WRT, ARG>::Apply(UO.arg())
                      );
  }

  static inline ReturnType Apply(const ArgT& UO, const Overloader<4>& Ignored)  
  {
    return ReturnType(
                      FactorT(
                              Scalar(static_cast<NumericalType>(m)
                                     /
                                     static_cast<NumericalType>(n)),
                              ModPowArgT(UO.arg())
                              ),
                      DiffImpl<WRT, ARG>::Apply(UO.arg())
                      );
  }
};



  
} // namespace Differentiation  
} // namespace Daixt 


#endif

