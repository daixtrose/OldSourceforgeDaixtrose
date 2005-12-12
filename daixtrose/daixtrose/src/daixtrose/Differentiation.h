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

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/identity.hpp"
#include "boost/mpl/lower_bound.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/find.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/pair.hpp"
#include "boost/mpl/bool.hpp"
//
//
#include "boost/mpl/lambda.hpp"

#include "boost/type_traits/is_same.hpp"

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

// getting tired of typing 
namespace mpl = boost::mpl;

////////////////////////////////////////////////////////////////////////////////
// Prediff: this is some preprocessing: if for Diff(a, b) a does not contain b
// then we branch for zero immediately (hoping for speedup in compile-time)
template<class WRT, class ARG>
struct PreDifferentiator
{
  template <int i, int Dummy = 0> struct Overloader;

  typedef typename 
  mpl::if_c
  <
    true, //StaticOccurrenceCounter<ARG, WRT>::Result, 
    mpl::pair<ARG, 
              Overloader<1, 0> >, 
    mpl::pair<IsNull<typename Daixt::disambiguation<ARG>::type>,
              Overloader<0, 0> >
  >::type Decision;


  typedef typename mpl::first<Decision>::type ReturnType;
  typedef typename mpl::second<Decision>::type Overload;

  static inline ReturnType Apply(const ARG& arg)
  {
    return Overload::Apply(arg);
  }


  // return zero
  template <int Dummy> 
  struct Overloader<0, Dummy> 
  {
    static inline ReturnType Apply(const ARG& arg)
    {
      return ReturnType();
    }
  };
  
  // simply reach through
  template <int Dummy> 
  struct Overloader<1, Dummy> 
  {
    static inline ReturnType const & Apply(const ARG& arg)
    {
      return arg;
    }
  };
};


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
    COMPILE_TIME_ASSERT(SAME_TYPE(typename Daixt::disambiguation<WRT>::type, 
                                  typename Daixt::disambiguation<ARG>::type));

    return DiffImpl<WRT, ARG>::Apply(arg); 
  }
};


////////////////////////////////////////////////////////////////////////////////
// function for convenient call
template<class WRT, class ARG> 
inline 
Daixt::Expr
<
  typename DiffImpl<
                    WRT, 
                    typename PreDifferentiator<WRT, ARG>::ReturnType
                    >::ReturnType
>
Diff(const ARG& arg, const WRT& wrt)
{
  typedef typename PreDifferentiator<WRT, ARG>::ReturnType PreprocessedType;

  typedef Daixt::Expr<typename DiffImpl<WRT, PreprocessedType>::ReturnType> 
    ReturnType;

  return ReturnType(
                    Differentiator<WRT>()
                    (
                     PreDifferentiator<WRT, ARG>::Apply(arg)
                     )
                    );
}


////////////////////////////////////////////////////////////////////////////////
// implementation and some important specializations for DiffImpl


////////////////////////////////////////////////////////////////////////////////
// default: compare WRT and ARG and return 0 or 1
template<class WRT, class ARG> struct DiffImpl
{
  typedef typename Daixt::disambiguation<ARG>::type Disambiguation;

  enum { Condition = SAME_TYPE(WRT, ARG) };
  
  typedef typename 
  mpl::eval_if_c<
    Condition, 
    mpl::identity<IsOne<Disambiguation> >, 
    mpl::identity<IsNull<Disambiguation> >
  >::type ReturnType;

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
  mpl::eval_if_c<(
                    SAME_TYPE(OP, Daixt::DefaultOps::UnaryPlus)
                    ||
                    SAME_TYPE(OP, Daixt::DefaultOps::UnaryMinus)
                    ),
                   mpl::identity<Daixt::UnOp<ArgReturnType, OP> >,
                   mpl::identity<XXX_____________Sorry_Not_Implemented_Yet_________________XXX>
               >::type ReturnType;


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
    mpl::eval_if_c<(
                      SAME_TYPE(OP, Daixt::DefaultOps::BinaryPlus)
                      ||
                      SAME_TYPE(OP, Daixt::DefaultOps::BinaryMinus)
                      ),
                     mpl::identity<Daixt::BinOp<DiffedLHS, DiffedRHS, OP> >,
                     mpl::identity<XXX_____________Sorry_Not_Implemented_Yet_________________XXX>
                 >::type ReturnType;


  
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
  typedef Daixt::IsNull<typename Daixt::disambiguation<RHS>::type> Null;
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
  typedef Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<mm, nn> > ArgT;
  typedef typename Daixt::disambiguation<ArgT>::type Disambiguation;

  typedef Daixt::Scalar<Disambiguation> Scalar;
  typedef typename Scalar::NumericalType NumericalType;

  // Now we go astray with our basic design where we completely separate 
  // expression simplifying from differentiation rules and simplify 
  // this expression a little bit:
  // if (nn < 0) we multiply mm and nn with (-1) in order to have some
  // nice values
  enum { m = (nn < 0) ? -mm : mm, 
         n = (nn < 0) ? -nn : nn };

private:
  // we want to mutiply the result with Scalar(m)/Scalar(n), so 1/n must be possible
  COMPILE_TIME_ASSERT(!std::numeric_limits<NumericalType>::is_integer);
  // negative values must be possible
  COMPILE_TIME_ASSERT(std::numeric_limits<NumericalType>::is_signed);

  // paranoia: maybe someone calls this in a recursive way
  // not accepted by intel's C++: ((m-n) > std::numeric_limits<int>::min());
  COMPILE_TIME_ASSERT((n>0) && ((m-n) < m));
  COMPILE_TIME_ASSERT(n != 0); // paranoia

  //////////////////////////////////////////////////////////////////////////////
  // some type calculation wizardry for ScalarFactorT


  //   if (m/n == 1)  ScalarFactorT is One 
  //   if (m/n == -1) ScalarFactorT is -One 
  //   if (m == 1)    ScalarFactorT is Scalar(n)^(-1)
  //   if (m != 1) but (m < 0) ScalarFactorT is  - Scalar(-m)
  //   else ScalarFactorT = Scalar(m/n)

  // emulating a "switch" on bools (pay attention, order matters!)
  template <int i, int Dummy = 0> struct Overloader;

  typedef mpl::vector
  <
    mpl::pair<bool_<m == n >, 
              mpl::pair<IsOne<Disambiguation>, 
                        Overloader<0, 0> > >,

    mpl::pair<bool_<m == -n>, 
              mpl::pair<Daixt::UnOp<IsOne<Disambiguation>, 
                                    Daixt::DefaultOps::UnaryMinus>,
                        Overloader<1, 0> > >,
    
    mpl::pair<bool_<m == 1 >, 
              mpl::pair<Daixt::UnOp<Scalar, 
                                    Daixt::DefaultOps::RationalPower<-1, 1> >,
                        Overloader<2, 0> > >,
    
    mpl::pair<bool_<(m < 0)  >,
              mpl::pair<Daixt::UnOp<Scalar, 
                                    Daixt::DefaultOps::UnaryMinus>,
                        Overloader<3, 0> > >,
  
    mpl::pair<bool_<true   >, 
              mpl::pair<Scalar,
                        Overloader<4, 0> > >
  > decision_sequence;
  

  typedef typename mpl::find_if<
    decision_sequence,
    boost::is_same<mpl::first<mpl::_1>, 
                   mpl::bool_<true> > > 
  iter_;

  typedef typename iter_::type::type pair_;
  typedef typename mpl::second<pair_>::type decision;
  
public:
  typedef typename mpl::first<decision>::type ScalarFactorT;
  typedef typename mpl::second<decision>::type Overload;

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
    return Overload::Apply(UO);
  }

private:
  template <int Dummy> 
  struct Overloader<0, Dummy> 
  {
    static inline ReturnType Apply(const ArgT& UO, const Overloader<0, 0>& Ignored)  
    {
      typedef IsOne<Disambiguation> One;
      
      return ReturnType(
                        FactorT(
                                One(),
                                ModPowArgT(UO.arg())
                                ),
                        DiffImpl<WRT, ARG>::Apply(UO.arg())
                        );
    }  
  };



  template <int Dummy> 
  struct Overloader<1, Dummy> 
  {
    static inline ReturnType Apply(const ArgT& UO)  
    {
      typedef IsOne<Disambiguation> One;
      typedef Daixt::UnOp<One, Daixt::DefaultOps::UnaryMinus> MinusOne;
      
      return ReturnType(
                        FactorT(
                                MinusOne(One()),
                                ModPowArgT(UO.arg())
                                ),
                        DiffImpl<WRT, ARG>::Apply(UO.arg())
                        );
    }  
  };


  template <int Dummy> 
  struct Overloader<2, Dummy> 
  {
    static inline ReturnType Apply(const ArgT& UO)  
    {
      return ReturnType(
                        FactorT(
                                InverseOfN(Scalar(static_cast<NumericalType>(n))),
                                ModPowArgT(UO.arg())
                                ),
                        DiffImpl<WRT, ARG>::Apply(UO.arg())
                        );
    }
  };


  template <int Dummy> 
  struct Overloader<3, Dummy> 
  {
    static inline ReturnType Apply(const ArgT& UO)  
    {
      typedef Daixt::UnOp<Scalar, Daixt::DefaultOps::UnaryMinus > MinusScalar;
      
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
  };


  template <int Dummy> 
  struct Overloader<4, Dummy> 
  {
    static inline ReturnType Apply(const ArgT& UO)  
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
};



  
} // namespace Differentiation  
} // namespace Daixt 


#endif

