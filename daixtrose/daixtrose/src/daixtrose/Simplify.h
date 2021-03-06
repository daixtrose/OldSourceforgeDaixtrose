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

#ifndef DAIXT_SIMPLIFY_INC
#define DAIXT_SIMPLIFY_INC

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/identity.hpp"

#include "daixtrose/CompileTimeChecks.h"
#include "daixtrose/MatrixSelect.h"
#include "daixtrose/NeutralElements.h"
#include "daixtrose/DefaultOps.h"

#include <utility>


namespace Daixt 
{

namespace ExprManip
{


using namespace ::boost::mpl;
using std::pair;

////////////////////////////////////////////////////////////////////////////////
// Simplifier is able to check at compile time whether a operation
// can be simplified, e.g. a * 0 = 0 or a + 0 = a
////////////////////////////////////////////////////////////////////////////////

// FIXIT: Yet incomplete. Remark: This file is very likely to evolve a lot,
// since there is an uncountable number of possibilities how to simplify an
// expression. As of today only the most simple cases are addressed here, but of
// course it is simple to _add_ (!) other features simply by specializing
// SimplImpl<T>. It's so _simple_!

// FIXIT: Missing: simplifier for logical operations (anyone out there?)

////////////////////////////////////////////////////////////////////////////////
// SimplEvoker calls SimplImpl recursively until no further simplification is
// possible. SimplImpl then again calls SimplEvoker on all branches of an
// expression. This little recursive Ping-Pong guarantees maximum
// simplification.

template <class T> class SimplEvoker;

////////////////////////////////////////////////////////////////////////////////
// The one to call. Here we first get rid of any outer Expr<> wrapper,
// then we proceed and the result again is wrapped into an Expr<>
template <class T> struct Simplifier
{
  typedef typename Daixt::UnwrapExpr<T>::Type UnwrappedT;
  typedef typename 
    SimplEvoker<UnwrappedT>::type SimplifiedUnwrappedT; 
  typedef Expr<SimplifiedUnwrappedT> type;

  static inline type Apply(const T& t)
  {
    return type(SimplEvoker<UnwrappedT>::Apply(Daixt::unwrap_expr(t)));
  }
};


////////////////////////////////////////////////////////////////////////////////
// if T1 and T2 differ, we repeat the simplification process, else we stop it

// boost mpl may help to put it into one class, which fails here

template <class T1, class T2, class TheSimplifier>
struct RepeatSimplifier 
{
  typedef typename TheSimplifier::type type;
};


template <class T, class TheSimplifier> 
struct RepeatSimplifier<T, T, TheSimplifier>
{
  typedef T type;
};


////////////////////////////////////////////////////////////////////////////////
template <class T> struct SimplImpl;


////////////////////////////////////////////////////////////////////////////////
template <class T>
class SimplEvoker
{
private:  
  template <class C> struct Hint {};

public:
  typedef typename SimplImpl<T>::type Simpler1;

  typedef typename 
  boost::mpl::eval_if_c<SAME_TYPE(T, Simpler1), 
             boost::mpl::identity<T>, 
             SimplImpl<Simpler1> >::type Simpler2;

  typedef typename 
  boost::mpl::eval_if_c<SAME_TYPE(Simpler1, Simpler2),
             boost::mpl::identity<Simpler1>,
             Daixt::ExprManip::SimplEvoker<Simpler2> >::type SimplerN;


  // Here we decide upon whether we stop the simplification process or whether
  // we refine until convergence.  So this is the dangerous part, since if one
  // makes an error below it may happen that we have a periodic behaviour
  // switching between several versions of the expression.

private:
  template <int i, int Dummy = 0> struct Overloader {};
  
#if defined(__HP_aCC)
  typedef boost::mpl::identity<pair<Simpler1, Overloader<1, 0> > > Option_1;
  typedef boost::mpl::identity<pair<Simpler2, Overloader<2, 0> > > Option_2;
  typedef boost::mpl::identity<pair<SimplerN, Overloader<3, 0> > > Option_3;

  typedef typename 
  boost::mpl::eval_if_c<
    (SAME_TYPE(Simpler1, Simpler2) 
     || 
     SAME_TYPE(Simpler1, SimplerN)),
    
    Option_1,
    
    boost::mpl::eval_if_c<SAME_TYPE(Simpler2, SimplerN),
                           Option_2,
                           Option_3
                           >
  >::type Decision;

#else
  typedef typename 
  boost::mpl::eval_if_c<
             SAME_TYPE(T, Simpler1),
             
             boost::mpl::identity<pair<T, Overloader<0, 0> > >, 
             
             boost::mpl::eval_if_c<
                        (SAME_TYPE(Simpler1, Simpler2) 
                         || 
                         SAME_TYPE(Simpler1, SimplerN)),
             
                        boost::mpl::identity<pair<Simpler1, Overloader<1, 0> > >,
                        
                        boost::mpl::eval_if_c<
                                   SAME_TYPE(Simpler2, SimplerN),
                                    
                                   boost::mpl::identity<pair<Simpler2, Overloader<2, 0> > >,
                                    
                                   boost::mpl::identity<pair<SimplerN, Overloader<3, 0> > >
                                   >
                        >
             > ::type Decision;
#endif

  // paranoia: of course Disambiguation must not change, so we check
  COMPILE_TIME_ASSERT
  (SAME_TYPE
   (typename Daixt::disambiguation<T>::type, 
    typename Daixt::disambiguation<typename Decision::first_type>::type));

  // Overload disambiguation: this is necessary, because SimplerA and SimplerB
  // may have same type, and we would get multiple equivalent overloads with
  // Hint<Simpler*>

  typedef typename Decision::second_type Overload;

public:
  typedef typename Decision::first_type type;

  static inline type Apply(const T& t)
  {
    return type(Apply(t, Overload())); 
  }

private:
  // overload on return type 
  static inline T Apply(const T& t, const Overloader<0, 0>& Ignored)
  {
    return t; 
  }

  static inline Simpler1 Apply(const T& t, const Overloader<1, 0>& Ignored)
  {
    return SimplImpl<T>::Apply(t); 
  }

  static inline Simpler2 Apply(const T& t, const Overloader<2, 0>& Ignored)
  {
    Simpler1 Tmp1 = SimplImpl<T>::Apply(t);
    Simpler2 Tmp2 = SimplImpl<Simpler1>::Apply(Tmp1);

    return Tmp2;
//     return SimplImpl<Simpler1>::Apply(SimplImpl<T>::Apply(t));
  }

  static inline SimplerN Apply(const T& t, const Overloader<3, 0>& Ignored)
  {
    return 
      SimplEvoker<Simpler2>::Apply(Apply(t, Overloader<2, 0>()));
  }

};


////////////////////////////////////////////////////////////////////////////////
// SimplImpl
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// default: no modification 
template <class T>
struct SimplImpl
{
  typedef T type;
  static const T& Apply(const T& t)
  {
    return t;
  }
};

////////////////////////////////////////////////////////////////////////////////
// Expr: if You reach here this is simply an error. You should use Simplifier

template <class T> 
struct SimplImpl<Daixt::Expr<T> >; // No no!


////////////////////////////////////////////////////////////////////////////////
// UnOp
////////////////////////////////////////////////////////////////////////////////

template <class ARG, class OP>
struct SimplImpl<Daixt::UnOp<ARG, OP> >
{
private:
  typedef typename SimplEvoker<ARG>::type SimplifiedARG;
  typedef typename Daixt::UnOp<SimplifiedARG, OP> DefaultResultT;
  
  // special case: simplify(ARG) yields 0, then we give back a zero
  // (this may be inappropriate for soem UnOp, needs improvement)
  // Zero and Zero are not the same: we have to translate the disambiguation:
  typedef IsNull<typename Daixt::disambiguation<ARG>::type> ARGNull;
  typedef Daixt::UnOp<ARG, OP> ArgT;
  typedef IsNull<typename Daixt::UnOp<ARG, OP>::Disambiguation> Null;


  //////////////////////////////////////////////////////////////////////////////
  // Several possibilities for the return value/type:
  // 0                    if (SimplifiedARG == 0)
  // DefaultResultT        in all other cases checked here
  struct Error {};

  template <int i, int Dummy = 0> struct Overloader {};

#if defined(__HP_aCC)
  typedef boost::mpl::identity<pair<Null, Overloader<0, 0> > > Option_0;
  typedef boost::mpl::identity<pair<DefaultResultT, Overloader<1, 0> > > Option_1;

  typedef typename boost::mpl::eval_if_c<SAME_TYPE(SimplifiedARG, ARGNull), 
                                          Option_0, Option_1
                                          >::type Decision;

#else
  typedef typename boost::mpl::eval_if_c<
                            SAME_TYPE(SimplifiedARG, ARGNull), 
                            boost::mpl::identity<pair<Null, Overloader<0, 0> > >,
                            boost::mpl::identity<pair<DefaultResultT, Overloader<1, 0> > >
                            >::type Decision;
#endif

  typedef typename Decision::second_type Overload;

public:
  typedef typename Decision::first_type type;
  
  static type Apply(const ArgT& UO)
  {
    return Apply(UO, Overload());
  }


private:
  static Null Apply(const ArgT& UO, const Overloader<0, 0>& Ignored)
  {
    return Null();
  }

  static DefaultResultT Apply(const ArgT& UO, const Overloader<1, 0>& Ignored)
  {
    return DefaultResultT(SimplEvoker<ARG>::Apply(UO.arg()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// -(-x) = x
template <class ARG>
struct SimplImpl<Daixt::UnOp<Daixt::UnOp<ARG, 
                                       Daixt::DefaultOps::UnaryMinus>,
                            Daixt::DefaultOps::UnaryMinus> >
{
  typedef ARG type;

  typedef Daixt::UnOp<Daixt::UnOp<ARG, 
                                Daixt::DefaultOps::UnaryMinus>,
                     Daixt::DefaultOps::UnaryMinus> ArgT;

  static type Apply(const ArgT& UO)
  {
    return UO.arg().arg();
  }
};

////////////////////////////////////////////////////////////////////////////////
// Daixt::DefaultOps::RationalPower 
template <int mm, int nn, class ARG>
struct SimplImpl<Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<mm, nn> > >
{
private:
  COMPILE_TIME_ASSERT(nn != 0); // paranoia

  // if (nn < 0) we multiply mm and nn with (-1) in order to have some
  // nice values
  enum { m = (nn < 0) ? -mm : mm, 
         n = (nn < 0) ? -nn : nn };

  COMPILE_TIME_ASSERT((n>0) && ((m-n) < m)); // the (n>0) test is unnecessary,
                                             // but code robust against changes

  typedef typename SimplEvoker<ARG>::type SimplifiedARG;

  typedef Daixt::UnOp<SimplifiedARG, 
                     Daixt::DefaultOps::RationalPower<m, n> > DefaultResultT;


  //////////////////////////////////////////////////////////////////////////////
  // Several possibilities for the return value/type:
  // 0                    if (ARG == 0)
  // 1                    if (ARG == 1 || m == 0)  
  // SimplifiedARG        if (m == n)
  // DefaultResultT        in all other cases checked here
  
  typedef IsNull<typename Daixt::disambiguation<ARG>::type> ARGNull;
  typedef IsOne<typename Daixt::disambiguation<ARG>::type> ARGOne;

  typedef Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<mm, nn> > ArgT;

  // possible return value: 0 or 1, but has to have appropriate diambiguation
  typedef IsNull<typename Daixt::disambiguation<ArgT>::type> Null;
  typedef IsOne<typename Daixt::disambiguation<ArgT>::type> One;


  // RationalPower<0, n>(0) is not defined
  COMPILE_TIME_ASSERT(!(SAME_TYPE(ARG, ARGNull) && (mm == 0))); 


  template <int i, int Dummy = 0> struct Overloader {};


#if defined(__HP_aCC)
  typedef boost::mpl::identity<pair<Null, Overloader<0, 0> > > Option_0;
  typedef boost::mpl::identity<pair<One, Overloader<1, 0> > > Option_1;
  typedef boost::mpl::identity<pair<SimplifiedARG, Overloader<2, 0> > > Option_2;
  typedef boost::mpl::identity<pair<DefaultResultT, Overloader<3, 0> > > Option_3;

  typedef typename 
  boost::mpl::eval_if_c<
           ((m == 0) || SAME_TYPE(ARG, ARGOne)),
           Option_1,
           boost::mpl::eval_if_c<
                      SAME_TYPE(ARG, ARGNull),
                      Option_0,
                      boost::mpl::eval_if_c<
                                 (m == n),
                                 Option_2,
                                 Option_3
                                 >             
                    >          
           >::type Decision;

#else
  typedef typename 
  boost::mpl::eval_if_c<
           ((m == 0) || SAME_TYPE(ARG, ARGOne)),
           boost::mpl::identity<pair<One, Overloader<1, 0> > >,
           boost::mpl::eval_if_c<
                      SAME_TYPE(ARG, ARGNull),
                      boost::mpl::identity<pair<Null, Overloader<0, 0> > >,
                      boost::mpl::eval_if_c<
                                 (m == n),
                                 boost::mpl::identity<pair<SimplifiedARG, Overloader<2, 0> > >,
                                 boost::mpl::identity<pair<DefaultResultT, Overloader<3, 0> > >
                                 >             
                    >          
           >::type Decision;
#endif

  typedef typename Decision::second_type Overload;
  
public:
  typedef typename Decision::first_type type;
  
  static type Apply(const ArgT& UO)
  {
    return Apply(UO, Overload());
  }
  

private:
  static Null Apply(const ArgT& UO, const Overloader<0, 0>& Ignored)
  {
    return Null();
  }

  static One Apply(const ArgT& UO, const Overloader<1, 0>& Ignored)
  {
    return One();
  }

  static SimplifiedARG Apply(const ArgT& UO, const Overloader<2, 0>& Ignored)
  {
    return SimplEvoker<ARG>::Apply(UO.arg());
  }

  static DefaultResultT Apply(const ArgT& UO, const Overloader<3, 0>& Ignored)
  {
    return DefaultResultT(SimplEvoker<ARG>::Apply(UO.arg()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// (x^a)^b = x^(a*b)
template <int m, int n, int o, int p, class ARG>
struct SimplImpl<
  Daixt::UnOp<Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<m, n> >, 
             Daixt::DefaultOps::RationalPower<o, p> > > 
{
  typedef typename SimplEvoker<ARG>::type SimplifiedARG;

  typedef Daixt::UnOp<SimplifiedARG, 
                     Daixt::DefaultOps::RationalPower<m * o, n * p> > type;

  typedef Daixt::UnOp<Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<m, n> >, 
                     Daixt::DefaultOps::RationalPower<o, p> > ArgT;

  static type Apply(const ArgT& UO)
  {
    return type(SimplEvoker<ARG>::Apply(UO.arg().arg()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// (x*y)^a = x^a * y^a
template <int m, int n, class LHS, class RHS>
struct SimplImpl<
  Daixt::UnOp<Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply>, 
             Daixt::DefaultOps::RationalPower<m, n> > > 

{
  typedef typename SimplEvoker<LHS>::type SimplifiedLHS;
  typedef typename SimplEvoker<RHS>::type SimplifiedRHS;


  typedef Daixt::UnOp<SimplifiedLHS, 
                     Daixt::DefaultOps::RationalPower<m, n> > PowLHS;
  typedef Daixt::UnOp<SimplifiedRHS, 
                     Daixt::DefaultOps::RationalPower<m, n> > PowRHS;


  typedef Daixt::BinOp<PowLHS, PowRHS, Daixt::DefaultOps::BinaryMultiply> type;

  typedef Daixt::UnOp<Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply>, 
                     Daixt::DefaultOps::RationalPower<m, n> > ArgT;

  static type Apply(const ArgT& UO)
  {
//     PowLHS plhs = PowLHS(SimplEvoker<LHS>::Apply(UO.arg().lhs()));
//     PowRHS prhs = PowRHS(SimplEvoker<RHS>::Apply(UO.arg().rhs()));

//     return type(plhs, prhs);
    return type(PowLHS(UO.arg().lhs()), PowRHS(UO.arg().rhs()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOp
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// BinaryMultiply: simplify the branches, check for 0 and 1 and return 
template <class LHS, class RHS>
struct SimplImpl<Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> >
{
private:
  typedef typename SimplEvoker<LHS>::type Simplified_LHS;
  typedef typename SimplEvoker<RHS>::type Simplified_RHS;

  typedef Daixt::DefaultOps::BinaryMultiply BinaryMultiply;
  
  typedef Daixt::BinOp<Simplified_LHS, 
                      Simplified_RHS, 
                      BinaryMultiply> DefaultResultT;

  // due to disambiguation we have 3 different types representing 0 and 1
  typedef typename 
  Daixt::BinOp<LHS, RHS, BinaryMultiply>::Disambiguation Disambiguation;
  typedef Daixt::IsNull<Disambiguation> Null;
  typedef Daixt::IsOne<Disambiguation> One;

  typedef Daixt::IsNull<typename Daixt::disambiguation<LHS>::type> LHSNull;
  typedef Daixt::IsOne<typename Daixt::disambiguation<LHS>::type> LHSOne;
  typedef Daixt::IsNull<typename Daixt::disambiguation<RHS>::type> RHSNull;
  typedef Daixt::IsOne<typename Daixt::disambiguation<RHS>::type> RHSOne;
  
  //////////////////////////////////////////////////////////////////////////////
  // Several possibilities for the return value/type:
  // 0                    if (lhs == 0 || rhs == 0) 
  // 1                    if (lhs == 1 && rhs == 1)  
  // Simplified_LHS       if (lhs != 0 && rhs == 1)
  // Simplified_RHS       if (rhs != 0 && lhs == 1)
  // DefaultResultT        in all other cases checked here

  template <int i, int Dummy = 0> struct Overloader {};
  
  static const bool result_is_null = 
    (SAME_TYPE(LHSNull, Simplified_LHS) || SAME_TYPE(RHSNull, Simplified_RHS));

  static const bool lhs_is_one = SAME_TYPE(LHSOne, Simplified_LHS);
  static const bool rhs_is_one = SAME_TYPE(RHSOne, Simplified_RHS);

#if defined(__HP_aCC)

  typedef boost::mpl::identity<pair<Null, Overloader<0, 0> > >           Option_0;  
  typedef boost::mpl::identity<pair<One, Overloader<1, 0> > >            Option_1;
  typedef boost::mpl::identity<pair<Simplified_RHS, Overloader<2, 0> > > Option_2;
  typedef boost::mpl::identity<pair<Simplified_LHS, Overloader<3, 0> > > Option_3;
  typedef boost::mpl::identity<pair<DefaultResultT, Overloader<4, 0> > > Option_4; 

  typedef typename 
  boost::mpl::eval_if_c<
             result_is_null,
             
             Option_0, 
             
             boost::mpl::eval_if_c<
                        lhs_is_one,
               
                        boost::mpl::eval_if_c<
                                   rhs_is_one,
                                   Option_1,
                                   Option_2
                                   >,
               
                        boost::mpl::eval_if_c<
                                   rhs_is_one,
                                   Option_3,
                                   Option_4
                                   >
                        >
             >::type Decision;

#else

  typedef typename 
  boost::mpl::eval_if_c<
             result_is_null,
             
             boost::mpl::identity<pair<Null, Overloader<0, 0> > >, 
             
             boost::mpl::eval_if_c<
                        lhs_is_one,
               
                        boost::mpl::eval_if_c<
                                   rhs_is_one,
                                   
                                   boost::mpl::identity<pair<One, Overloader<1, 0> > >,
                                   
                                   boost::mpl::identity<pair<Simplified_RHS, Overloader<2, 0> > >
                                   >,
               
                        boost::mpl::eval_if_c<
                                   rhs_is_one,
                                   
                                   boost::mpl::identity<pair<Simplified_LHS, Overloader<3, 0> > >,
                                   
                                   boost::mpl::identity<pair<DefaultResultT, Overloader<4, 0> > >
                                   >
                        >
             >::type Decision;

#endif

  typedef typename Decision::second_type Overload;

public:
  typedef typename Decision::first_type type;
  typedef Daixt::BinOp<LHS, RHS, BinaryMultiply> ArgT;

  static type Apply(const ArgT& BO)
  {
    return Apply(BO, Overload());
  }


private:
  static Null Apply(const ArgT& BO,
                    const Overloader<0, 0>& Ignored)
  {
    return Null();
  }


  static One Apply(const ArgT& BO, const Overloader<1, 0>& Ignored)
  {
    return One();
  }


  static Simplified_RHS Apply(const ArgT& BO, const Overloader<2, 0>& Ignored)
  {
    return SimplEvoker<RHS>::Apply(BO.rhs()); 
  }


  static Simplified_LHS Apply(const ArgT& BO, const Overloader<3, 0>& Ignored)
  {
    return SimplEvoker<LHS>::Apply(BO.lhs());
  }


  static DefaultResultT Apply(const ArgT& BO, const Overloader<4, 0>& Ignored)
  {
    return DefaultResultT(SimplEvoker<LHS>::Apply(BO.lhs()),
                          SimplEvoker<RHS>::Apply(BO.rhs()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinaryMultiply: a * (-b) = - (a * b)
template <class LHS, class RHS>
struct SimplImpl<Daixt::BinOp<LHS, 
                             Daixt::UnOp<RHS, Daixt::DefaultOps::UnaryMinus>, 
                             Daixt::DefaultOps::BinaryMultiply> >
{
  typedef typename SimplEvoker<LHS>::type Simplified_LHS;
  typedef typename SimplEvoker<RHS>::type Simplified_RHS;

  typedef Daixt::DefaultOps::BinaryMultiply BinaryMultiply;
  typedef Daixt::DefaultOps::UnaryMinus UnaryMinus;

  typedef Daixt::BinOp<Simplified_LHS, 
                      Simplified_RHS, 
                      BinaryMultiply> MultiplyLHSRHS;

  typedef Daixt::UnOp<MultiplyLHSRHS, UnaryMinus> type;
  typedef Daixt::BinOp<LHS, Daixt::UnOp<RHS, UnaryMinus>, BinaryMultiply> ArgT;

  static type Apply(const ArgT& BO)
  {
    return type(MultiplyLHSRHS(SimplEvoker<LHS>::Apply(BO.lhs()),
                                 SimplEvoker<RHS>::Apply(BO.rhs().arg())));
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinaryMultiply: (-a) * b = - (a * b)
template <class LHS, class RHS>
struct SimplImpl<Daixt::BinOp<Daixt::UnOp<LHS, Daixt::DefaultOps::UnaryMinus>, 
                             RHS, 
                             Daixt::DefaultOps::BinaryMultiply> >
{
  typedef typename SimplEvoker<LHS>::type Simplified_LHS;
  typedef typename SimplEvoker<RHS>::type Simplified_RHS;

  typedef Daixt::DefaultOps::BinaryMultiply BinaryMultiply;
  typedef Daixt::DefaultOps::UnaryMinus UnaryMinus;

  typedef Daixt::BinOp<Simplified_LHS, 
                      Simplified_RHS, 
                      BinaryMultiply> MultiplyLHSRHS;

  typedef Daixt::UnOp<MultiplyLHSRHS, UnaryMinus> type;
  typedef Daixt::BinOp<Daixt::UnOp<LHS, UnaryMinus>, RHS, BinaryMultiply> ArgT;

  static type Apply(const ArgT& BO)
  {
    return type(MultiplyLHSRHS(SimplEvoker<LHS>::Apply(BO.lhs().arg()),
                                 SimplEvoker<RHS>::Apply(BO.rhs())));
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinaryMultiply: (-a) * (-b) = a * b
template <class LHS, class RHS>
struct SimplImpl<Daixt::BinOp<Daixt::UnOp<LHS, Daixt::DefaultOps::UnaryMinus>, 
                             Daixt::UnOp<RHS, Daixt::DefaultOps::UnaryMinus>,
                             Daixt::DefaultOps::BinaryMultiply> >
{
  typedef typename SimplEvoker<LHS>::type Simplified_LHS;
  typedef typename SimplEvoker<RHS>::type Simplified_RHS;

  typedef Daixt::DefaultOps::BinaryMultiply BinaryMultiply;
  typedef Daixt::DefaultOps::UnaryMinus UnaryMinus;

  typedef Daixt::BinOp<Simplified_LHS, 
                      Simplified_RHS, 
                      BinaryMultiply> type;

  typedef Daixt::BinOp<Daixt::UnOp<LHS, UnaryMinus>,
                      Daixt::UnOp<RHS, UnaryMinus>, 
                      BinaryMultiply> ArgT;

  static type Apply(const ArgT& BO)
  {
    return type(SimplEvoker<LHS>::Apply(BO.lhs().arg()),
                  SimplEvoker<RHS>::Apply(BO.rhs().arg()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinaryDivide: forward to BinaryMultiply: a / b = a * b^(-1)
template <class LHS, class RHS>
struct SimplImpl<Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryDivide> >
{
private:
  // check for division by zero                                               
  typedef Daixt::IsNull<typename Daixt::disambiguation<RHS>::type> Null;
  COMPILE_TIME_ASSERT(!SAME_TYPE(RHS, Null));

public:
  typedef typename SimplEvoker<LHS>::type Simplified_LHS;
  typedef typename SimplEvoker<RHS>::type Simplified_RHS;

  typedef Daixt::DefaultOps::BinaryDivide BinaryDivide;
  
  typedef Daixt::UnOp<Simplified_RHS, Daixt::DefaultOps::RationalPower<-1, 1> > 
  InverseOf_Simplified_RHS ;

  typedef Daixt::BinOp<Simplified_LHS, 
                      InverseOf_Simplified_RHS, 
                      Daixt::DefaultOps::BinaryMultiply> type;

  static type Apply(const Daixt::BinOp<LHS, RHS, BinaryDivide>& BO)
  {
    return type(SimplEvoker<LHS>::Apply(BO.lhs()),
                  InverseOf_Simplified_RHS
                  (SimplEvoker<RHS>::Apply(BO.rhs())));
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinaryDivide: a / a = 1
template <class ARG>
struct SimplImpl<Daixt::BinOp<ARG, ARG, Daixt::DefaultOps::BinaryDivide> >
{
private:
  // check for division by zero                                               
  typedef Daixt::IsNull<typename Daixt::disambiguation<ARG>::type> Null;
  COMPILE_TIME_ASSERT(!SAME_TYPE(ARG, Null));

public:
  typedef Daixt::DefaultOps::BinaryDivide BinaryDivide;
  typedef typename
  Daixt::BinOp<ARG, ARG, BinaryDivide>::Disambiguation Disambiguation;
  typedef Daixt::IsOne<Disambiguation> type;

  static type Apply(const Daixt::BinOp<ARG, ARG, BinaryDivide>& BO)
  {
    return type();
  }
};



////////////////////////////////////////////////////////////////////////////////
// BinaryPlus: simplify the branches, check for 0 and 1 and return 
template <class LHS, class RHS>
struct SimplImpl<Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus> >
{
private:
  typedef typename SimplEvoker<LHS>::type Simplified_LHS;
  typedef typename SimplEvoker<RHS>::type Simplified_RHS;

  typedef Daixt::DefaultOps::BinaryPlus BinaryPlus;
  
  typedef Daixt::BinOp<Simplified_LHS, 
                      Simplified_RHS, 
                      BinaryPlus> DefaultResultT;

  typedef typename 
  Daixt::BinOp<LHS, RHS, BinaryPlus>::Disambiguation Disambiguation;

  typedef Daixt::IsNull<Disambiguation> Null;
  typedef Daixt::IsOne<Disambiguation> One;
  
  //////////////////////////////////////////////////////////////////////////////
  // Several possibilities for the return value/type:
  // 0                    if (lhs == 0 && rhs == 0) 
  // 1                    if (lhs == 1 && rhs == 0) || (lhs == 0 && rhs == 1)  
  // Simplified_LHS       if (lhs != 0 && rhs == 0)
  // Simplified_RHS       if (rhs != 0 && lhs == 0)
  // DefaultResultT        in all other cases checked here

  enum { LHS_IsNull = SAME_TYPE(Null, Simplified_LHS) };
  enum { RHS_IsNull = SAME_TYPE(Null, Simplified_RHS) };

  enum { LHS_IsOne = SAME_TYPE(One, Simplified_LHS) };
  enum { RHS_IsOne = SAME_TYPE(One, Simplified_RHS) };

  enum { ResultIsOne = 
    (LHS_IsOne && RHS_IsNull) || (LHS_IsNull && RHS_IsOne) };


  template <int i, int Dummy = 0> struct Overloader {};

#if defined(__HP_aCC)
  typedef boost::mpl::identity<pair<Null, Overloader<0, 0> > >           Option_0;
  typedef boost::mpl::identity<pair<One, Overloader<1, 0> > >            Option_1;
  typedef boost::mpl::identity<pair<Simplified_LHS, Overloader<2, 0> > > Option_2;
  typedef boost::mpl::identity<pair<Simplified_RHS, Overloader<3, 0> > > Option_3;
  typedef boost::mpl::identity<pair<DefaultResultT, Overloader<4, 0> > > Option_4;

  typedef typename
  boost::mpl::eval_if_c<
             ResultIsOne, // special case handled separately
             
             Option_1,
             
             boost::mpl::eval_if_c<
                        LHS_IsNull, 
                        boost::mpl::eval_if_c<
                                   RHS_IsNull, 
                                   Option_0, 
                                   Option_3
                                   >,
                        boost::mpl::eval_if_c<
                                   RHS_IsNull, 
                                   Option_2, 
                                   Option_4 
                                   >
                        >
             >::type Decision;

#else

  typedef typename
  boost::mpl::eval_if_c<
             ResultIsOne, // special case handled separately
             
             boost::mpl::identity<pair<One, Overloader<1, 0> > >,
             
             boost::mpl::eval_if_c<
                        LHS_IsNull, 
                        boost::mpl::eval_if_c<
                                   RHS_IsNull, 
                                   boost::mpl::identity<pair<Null, Overloader<0, 0> > >, 
                                   boost::mpl::identity<pair<Simplified_RHS, Overloader<3, 0> > > 
                                   >,
                        boost::mpl::eval_if_c<
                                   RHS_IsNull, 
                                   boost::mpl::identity<pair<Simplified_LHS, Overloader<2, 0> > >, 
                                   boost::mpl::identity<pair<DefaultResultT, Overloader<4, 0> > > 
                                   >
                        >
             >::type Decision;

#endif

  typedef typename Decision::second_type Overload;

public:
  typedef typename Decision::first_type type;  

  //////////////////////////////////////////////////////////////////////////////
  template <class C> struct Hint {};
  typedef Daixt::BinOp<LHS, RHS, BinaryPlus> ArgT;

  static type Apply(const ArgT& BO)
  {
    return Apply(BO, Overload());
  }


private:
  static Null Apply(const ArgT& BO, const Overloader<0, 0>& Ignored)
  {
    return Null();
  }


  static One Apply(const ArgT& BO, const Overloader<1, 0>& Ignored)
  {
    return One();
  }


  static Simplified_LHS Apply(const ArgT& BO, const Overloader<2, 0>& Ignored)
  {
    return SimplEvoker<LHS>::Apply(BO.lhs());
  }


  static Simplified_RHS Apply(const ArgT& BO, const Overloader<3, 0>& Ignored)
  {
    return SimplEvoker<RHS>::Apply(BO.rhs()); 
  }


  static DefaultResultT Apply(const ArgT& BO, const Overloader<4, 0>& Ignored)
  {
    return DefaultResultT(SimplEvoker<LHS>::Apply(BO.lhs()),
                          SimplEvoker<RHS>::Apply(BO.rhs()));
  }
};


////////////////////////////////////////////////////////////////////////////////
// x + (-x) = 0
template <class ARG>
struct SimplImpl<Daixt::BinOp
<ARG, 
 Daixt::UnOp<ARG, Daixt::DefaultOps::UnaryMinus>, 
 Daixt::DefaultOps::BinaryPlus> >
{
  typedef Daixt::BinOp<ARG, 
                       Daixt::UnOp<ARG, Daixt::DefaultOps::UnaryMinus>, 
                       Daixt::DefaultOps::BinaryPlus> ArgT;
  
  typedef IsNull<typename Daixt::disambiguation<ArgT>::type> Null;
  typedef Null type;

  static type Apply(const ArgT& BO)  
  {                                             
    return Null();
  }
};


////////////////////////////////////////////////////////////////////////////////
// x + x = 2 * x
template <class ARG>
struct SimplImpl<Daixt::BinOp<ARG, ARG, Daixt::DefaultOps::BinaryPlus> >
{
  typedef typename SimplEvoker<ARG>::type SimplifiedARG;
  typedef typename 
  Daixt::BinOp<ARG, ARG, 
              Daixt::DefaultOps::BinaryPlus>::Disambiguation Disambiguation;

  typedef Daixt::Scalar<Disambiguation> Scalar;
  typedef typename Scalar::NumericalType NumericalType;

  typedef Daixt::BinOp<Scalar, SimplifiedARG, 
                      Daixt::DefaultOps::BinaryMultiply> type; 


  static type Apply(const Daixt::BinOp<ARG, ARG, Daixt::DefaultOps::BinaryPlus>& BO)
  {
    return type(
                Scalar(static_cast<NumericalType>(2)),
                SimplEvoker<ARG>::Apply(BO.lhs())
                );
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinaryMinus forwards to BinaryPlus: (a - b) => a + (-b)
template <class LHS, class RHS>
struct SimplImpl<Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> >
{
  typedef Daixt::UnOp<RHS, Daixt::DefaultOps::UnaryMinus> MinusRHS;
  typedef Daixt::BinOp<LHS, MinusRHS, Daixt::DefaultOps::BinaryPlus> type;

  typedef Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> ArgT;

  static type Apply(const ArgT& BO)
  {
    return type(BO.lhs(), MinusRHS(BO.rhs()));
  }
};



////////////////////////////////////////////////////////////////////////////////
// Scalars
////////////////////////////////////////////////////////////////////////////////

// Problem: distinct values do not differ in type, so we have to
// overload/specialize the behaviour to achieve a correct behaviour.

#define SPECIALIZE_FOR_SCALARS(OPNAME, OP)                                    \
template <class Disambiguation>                                               \
struct SimplImpl<Daixt::BinOp<Daixt::Scalar<Disambiguation>,                  \
                             Daixt::Scalar<Disambiguation>,                   \
                             OPNAME> >                                        \
{                                                                             \
  typedef Daixt::Scalar<Disambiguation> type;                                 \
                                                                              \
  typedef Daixt::BinOp<Daixt::Scalar<Disambiguation>,                         \
                      Daixt::Scalar<Disambiguation>,                          \
                      OPNAME> ArgT;                                           \
                                                                              \
  typedef typename Daixt::Scalar<Disambiguation>::NumericalType NumericalType;\
                                                                              \
  static type Apply(const ArgT& BO)                                           \
  {                                                                           \
    return type(                                                              \
                BO.lhs().Value()                                              \
                OP                                                            \
                BO.rhs().Value()                                              \
                );                                                            \
  }                                                                           \
}  

SPECIALIZE_FOR_SCALARS(Daixt::DefaultOps::BinaryPlus,     +);
SPECIALIZE_FOR_SCALARS(Daixt::DefaultOps::BinaryMinus,    -);
SPECIALIZE_FOR_SCALARS(Daixt::DefaultOps::BinaryMultiply, *);
SPECIALIZE_FOR_SCALARS(Daixt::DefaultOps::BinaryDivide,   /);


#undef SPECIALIZE_FOR_SCALARS


////////////////////////////////////////////////////////////////////////////////
// neutral types
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////

template <class T> 
inline
typename Daixt::ExprManip::Simplifier<T>::type Simplify(const T& t)
{
  return Daixt::ExprManip::Simplifier<T>::Apply(t);
}


} // namespace ExprManip
} // namespace Daixt 



#endif // DAIXT_SIMPLIFY_INC
