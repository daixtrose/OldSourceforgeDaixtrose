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

#ifndef DAIXT_LINALG_ROW_AND_COLUMN_EXTRACTORS_INC
#define DAIXT_LINALG_ROW_AND_COLUMN_EXTRACTORS_INC

#include "linalg/Disambiguation.h"

#include "daixtrose/Daixt.h"

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"

#include "boost/mpl/apply_if.hpp"
#include "boost/mpl/identity.hpp"


#include <algorithm>
#include <functional>

namespace Linalg
{

template<class Disambiguation> class RowExtractor;
template<class Disambiguation> class ColExtractor;

namespace Private
{

////////////////////////////////////////////////////////////////////////////////
// merging two (std::)maps

template <class MapType>
inline 
MapType &
Merge(MapType& Map, const MapType& ToAdd)
{
  MapType Result(Map);
  
  typedef typename MapType::const_iterator const_iterator;

  const_iterator end = ToAdd.end();
  for (const_iterator iter = ToAdd.begin(); iter!= end; ++iter) {
    typename MapType::iterator lb = Map.lower_bound(iter->first);
    
    // if we are sure, the entry already exists  ...
    if(lb != Map.end() && !(Map.key_comp()(iter->first, lb->first))) 
      {
        lb->second += iter->second;
      }
    else 
      {
        typedef typename MapType::value_type MVT;
        Map.insert(lb, *iter);
      }
  }
  
  return Map;
}

////////////////////////////////////////////////////////////////////////////////
// merge while applying a functor to rhs
template <class MapType, class OP>
inline 
MapType &
Merge(MapType& Map, const MapType& ToAdd, const OP& Op)
{
  MapType Result(Map);
  
  typedef typename MapType::const_iterator const_iterator;

  const_iterator end = ToAdd.end();
  for (const_iterator iter = ToAdd.begin(); iter!= end; ++iter) {
    typename MapType::iterator lb = Map.lower_bound(iter->first);
    
    // if we are sure, the entry already exists  ...
    if(lb != Map.end() && !(Map.key_comp()(iter->first, lb->first))) 
      {
        lb->second += Op(iter->second);
      }
    else 
      {
        typedef typename MapType::value_type MVT;
        Map.insert(lb, MVT(iter->first, Op(iter->second)));
      }
  }
  
  return Map;
}

//////////////////////////////////////////////////////////////////////////////
// a functor which sets the second entry in a pair
 
template <class MapType>
class SetSecondToValue : 
    public std::unary_function<typename MapType::value_type, void> 
{
public:
  typedef typename MapType::value_type::second_type value_type;

  inline SetSecondToValue(const value_type value) : value_(value) {}
  
  inline void 
  operator()(typename MapType::value_type& entry) 
  {
    entry.second = value_;
  }
  
private:
  value_type value_;
};


} // namespace Private



// Overloading is implemented by forwarding all calls to a yet-to-be-specialized
// template class OperatorDelimImpl.  Using this extra indirection avoids any
// brain damage due to lookup rules, function template overload, ADL, etc. If
// You need extra functionality, write it and include it.  All visible
// specializations of OperatorDelimImpl will be considered by the compiler.
template<class T, class ARG> struct OperatorDelimImpl;

// matrix forward declared
template <class T, class RowStorage, class Allocator> class Matrix;


////////////////////////////////////////////////////////////////////////////////
// row extractor for matrix expression
////////////////////////////////////////////////////////////////////////////////

template<class T>
struct RowExtractor<MatrixExpression<T> >
{
  RowExtractor(std::size_t i) : i_(i) {}

  template<class ARG> inline 
  typename T::RowStorage
  operator()(const ARG& arg) const
  {
    return 
      OperatorDelimImpl<
                        RowExtractor<MatrixExpression<T> >, 
                        typename Daixt::UnwrapExpr<ARG>::Type
                        >::Apply(Daixt::unwrap_expr(arg), i_);
  }

  // sometimes a const reference can be used as return type: short-circuit
  typedef Matrix<typename T::NumT, 
                 typename T::RowStorage,
                 typename T::Allocator> MatrixT;

  typedef Daixt::ConstRef<MatrixT> CRefToMat;

  inline 
  const typename T::RowStorage&
  operator()(const CRefToMat& arg) const
  {
    return static_cast<const MatrixT&>(arg)(i_);
  }

  inline 
  const typename T::RowStorage&
  operator()(const MatrixT& arg) const
  {
    return arg(i_);
  }


private:
  std::size_t i_;
};


// never reached? :
////////////////////////////////////////////////////////////////////////////////
// Daixt::ConstRef
// template <class T, class Arg>
// struct
// OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
//                   Daixt::ConstRef<Arg> >
// {
//   static inline
//   typename T::RowStorage
//   Apply(const Daixt::ConstRef<Arg>& arg,
//         std::size_t i)
//   {
//     return RowExtractor<MatrixExpression<T> >(i)(static_cast<const Arg&>(arg));
//   }
// };



////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: Apply Op to result of RowExtractor(UnOp.arg())
template<class T, class ARG, class Op>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, Op> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::UnOp<ARG, Op>& arg,
        std::size_t i)
  { 
    typedef typename T::NumT NumT;

    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::iterator iterator;

    RowStorage Row = RowExtractor<MatrixExpression<T> >(i)(arg.arg());

    iterator end = Row.end();
    for (iterator iter = Row.begin(); iter != end; ++iter)
      {
        iter->second = Op::Apply(iter->second, Daixt::Hint<NumT>());
      }

    return Row; 
  }
};


// UnaryMinus
template<class T, class ARG>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, Daixt::DefaultOps::UnaryMinus> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::UnOp<ARG, Daixt::DefaultOps::UnaryMinus>& arg,
        std::size_t i)
  { 
    typedef typename T::NumT NumT;

    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::iterator iterator;

    RowStorage Row = RowExtractor<MatrixExpression<T> >(i)(arg.arg());

    iterator end = Row.end();
    for (iterator iter = Row.begin(); iter != end; ++iter)
      {
        iter->second *= (-1);
      }

    return Row; 
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOps

// no default, if we use crazy OPs, we need to add further specializations

// matrix expression + matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::iterator iterator;
    
    RowStorage LHS_Result = 
      RowExtractor<MatrixExpression<T> >(i)(arg.lhs());
    const RowStorage& RHS_Result = 
      RowExtractor<MatrixExpression<T> >(i)(arg.rhs());
    
    return Private::Merge(LHS_Result, RHS_Result);
  }
};


// matrix expression - matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::iterator iterator;
    typedef typename RowStorage::const_iterator const_iterator;

    RowStorage LHS_Result = 
      RowExtractor<MatrixExpression<T> >(i)(arg.lhs());
    const RowStorage& RHS_Result = 
      RowExtractor<MatrixExpression<T> >(i)(arg.rhs());
    
    const_iterator end = RHS_Result.end();
    for (const_iterator iter = RHS_Result.begin(); iter!= end; ++iter) 
      {
        iterator lb = LHS_Result.lower_bound(iter->first);
    
        // if we are sure, the entry already exists  ...
        if(lb != LHS_Result.end() 
           && 
           !(LHS_Result.key_comp()(iter->first, lb->first))) 
          {
            lb->second -= iter->second;
          }
        else 
          {
            using namespace Daixt::DefaultOps;


            typedef typename RowStorage::value_type MVT;
            typedef typename MVT::second_type ST;
            LHS_Result.insert(lb, MVT(iter->first, ST(0)));
            LHS_Result[iter->first] -= iter->second;
          }
      }
  
    return LHS_Result;
    

//     return Private::Merge(LHS_Result, RHS_Result, std::negate<NumT>());
  }
};

// FIXIT: missing due to lack of time
// // matrix expression * matrix expression -> some work
// template<class T, class LHS, class RHS>
// struct
// OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
//                   Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> >
// {
//   static inline
//   typename T::RowStorage
//   Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply>& arg,
//         std::size_t i)
//   {
//     typedef typename T::NumT NumT;
//     typedef typename T::RowStorage RowStorage;
//     typedef typename RowStorage::iterator iterator;
    
//     RowStorage LHS_Result = 
//       RowExtractor<MatrixExpression<T> >(i)(arg.lhs());

//     iterator end = LHS_Result.end();
//     for (iterator 


//     RowStorage RHS_Result = 
//       ColExtractor<MatrixExpression<T> >(i)(arg.rhs());
//   }
// };


// matrix expression * scalar value
template<class T, class LHS>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, 
                              Daixt::Scalar<MatrixExpression<T> >, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<LHS, 
                          Daixt::Scalar<MatrixExpression<T> >, 
                          Daixt::DefaultOps::BinaryMultiply>& arg,
        std::size_t i)
  {
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::value_type Pair;    

    typedef typename RowStorage::iterator iterator;

    Daixt::Scalar<MatrixExpression<T> > Factor = arg.rhs();

    typename T::RowStorage Row = 
      RowExtractor<MatrixExpression<T> >(i)(arg.lhs());

#if (defined(__GNUC__)) // FIXIT, when gcc has bug removed
    using namespace Daixt::Convenience;
#else
    using Daixt::Convenience::operator*;
#endif

    iterator end = Row.end();
    for (iterator iter = Row.begin(); iter != end; ++iter)
      {
        iter->second *= arg.rhs().Value();
      }

    return Row;
  }
};


// scalar value * matrix expression
template<class T, class RHS>
struct
OperatorDelimImpl<RowExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<Daixt::Scalar<MatrixExpression<T> >, 
                              RHS, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<Daixt::Scalar<MatrixExpression<T> >, 
                          RHS, 
                          Daixt::DefaultOps::BinaryMultiply>& arg,
        std::size_t i)
  {
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::value_type Pair;    

    typedef typename RowStorage::iterator iterator;

    typename T::RowStorage Row = 
      RowExtractor<MatrixExpression<T> >(i)(arg.rhs());

#if (defined(__GNUC__)) // FIXIT, when gcc has bug removed
    using namespace Daixt::Convenience;
#else
    using Daixt::Convenience::operator*;
#endif


    iterator end = Row.end();
    for (iterator iter = Row.begin(); iter != end; ++iter)
      {
        iter->second *= arg.lhs().Value();
      }

    return Row;
  }
};
  

////////////////////////////////////////////////////////////////////////////////
// row extractor for vector expression
////////////////////////////////////////////////////////////////////////////////

template<class T> 
struct RowExtractor<VectorExpression<T> >
{
  RowExtractor(std::size_t i) : i_(i) {}

  template<class ARG> 
  inline
  typename T::NumT 
  operator()(const ARG& arg) const
  {
    return 
      OperatorDelimImpl<
                        RowExtractor<VectorExpression<T> >, 
                        typename Daixt::UnwrapExpr<ARG>::Type
                        >::Apply(Daixt::unwrap_expr(arg), i_);
  }

private:
  std::size_t i_;
};


// Vector
template <class T, class Allocator> class Vector;

template<class T>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Vector<typename T::NumT, typename T::Allocator> >
{
  static inline 
  typename T::NumT
  Apply(const Vector<typename T::NumT, typename T::Allocator>& arg,
        std::size_t i)
  {
    return arg(i);
  }
};


////////////////////////////////////////////////////////////////////////////////
// Daixt::ConstRef
template <class T, class Arg>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::ConstRef<Arg> >
{
  static inline
  typename T::NumT
  Apply(const Daixt::ConstRef<Arg>& arg,
        std::size_t i)
  {
    return RowExtractor<VectorExpression<T> >(i)(static_cast<const Arg&>(arg));
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: 
template<class T, class ARG, class Op>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::UnOp<ARG, Op> >
{
  static inline
  typename T::NumT Apply(const Daixt::UnOp<ARG, Op>& arg,
                         std::size_t i)
  { 
    typedef typename T::NumT NumT;
    return Op::Apply(RowExtractor<VectorExpression<T> >(i)(arg.arg()), 
                     Daixt::Hint<NumT>());
  }
};


// Daixt::DefaultOps::UnaryMinus
template<class T, class ARG>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::UnOp<ARG, Daixt::DefaultOps::UnaryMinus> >
{
  static inline
  typename T::NumT Apply(const Daixt::UnOp<ARG, Daixt::DefaultOps::UnaryMinus>& arg,
                         std::size_t i)
  { 
    typedef typename T::NumT NumT;
    NumT Tmp = RowExtractor<VectorExpression<T> >(i)(arg.arg());
    Tmp *= (-1);
    return Tmp;
  }
};

////////////////////////////////////////////////////////////////////////////////
// BinOps

// default: 
template<class T, class LHS, class RHS, class Op>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Op> >
{
  static inline
  typename T::NumT 
  Apply(const Daixt::BinOp<LHS, RHS, Op>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
//     NumT LHS_Result = RowExtractor<VectorExpression<T> >(i)(arg.lhs());
//     NumT RHS_Result = RowExtractor<VectorExpression<T> >(i)(arg.rhs());
    
    return Op::Apply(RowExtractor<VectorExpression<T> >(i)(arg.lhs()), 
                     RowExtractor<VectorExpression<T> >(i)(arg.rhs()), 
                     Daixt::Hint<NumT>());
  }
};


// Daixt::DefaultOps::BinaryPlus: 
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus> >
{
  static inline
  typename T::NumT 
  Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    NumT LHS_Result = RowExtractor<VectorExpression<T> >(i)(arg.lhs());
    NumT RHS_Result = RowExtractor<VectorExpression<T> >(i)(arg.rhs());

    using namespace Daixt::DefaultOps;
//     using Daixt::DefaultOps::operator+;

    NumT Result = LHS_Result + RHS_Result;
    return Result;
//     return (RowExtractor<VectorExpression<T> >(i)(arg.lhs())
//             +
//             RowExtractor<VectorExpression<T> >(i)(arg.rhs()));
  }
};

// Daixt::DefaultOps::BinaryMinus: 
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> >
{
  static inline
  typename T::NumT 
  Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    NumT LHS_Result = RowExtractor<VectorExpression<T> >(i)(arg.lhs());
    NumT RHS_Result = RowExtractor<VectorExpression<T> >(i)(arg.rhs());

    using namespace Daixt::DefaultOps;
    NumT Result = LHS_Result - RHS_Result;
    return Result;
//     return (RowExtractor<VectorExpression<T> >(i)(arg.lhs())
//             +
//             RowExtractor<VectorExpression<T> >(i)(arg.rhs()));
  }
};


// scalar value
template<class T>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::Scalar<VectorExpression<T> > >
{
  static inline
  typename T::NumT 
  Apply(const Daixt::Scalar<VectorExpression<T> >& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    return static_cast<NumT>(arg);
  }
};


// matrix expression * vector expression
// vector expression * scalar value 
// scalar value * vector expression 
namespace Private
{
// for pedantic compiler's sake: defined outside of next class below
enum ArgResultType { row_storage, numerical_type };
template <ArgResultType arg> struct TypeOf {}; 
}


template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<RowExtractor<VectorExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> >
{
private:
  // OK, here we have a compile-time dispatch: if LHS is a MatrixExpression we
  // must apply RowExtractor<MatrixExpression ...> to lhs() and proceed in a
  // completely different manner, since the return type of this action is a
  // RowStorage not a NumT.
  //
  // We use an overload disambiguation on private helper functions to reach our
  // goals. Feel free to shake Your head here. This is hardcore braindamage C++.

  template <class TT> struct Dispatcher;
  template <class TT> struct Dispatcher<MatrixExpression<TT> >
  {
    typedef Private::TypeOf<Private::row_storage> ReturnType;
  };

  template <class TT> struct Dispatcher<VectorExpression<TT> >
  {
    COMPILE_TIME_ASSERT(SAME_TYPE(TT, T)); 
    typedef Private::TypeOf<Private::numerical_type> ReturnType;
  };
  
  
public:
  typedef Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMultiply> ArgT;

  static inline
  typename T::NumT 
  Apply(const ArgT& arg, std::size_t i)
  {
    typedef typename 
      Dispatcher<typename LHS::Disambiguation>::ReturnType LHS_Dispatch;
 
    typedef typename 
      Dispatcher<typename RHS::Disambiguation>::ReturnType RHS_Dispatch;
      
    
    return Apply(arg, i, LHS_Dispatch(), RHS_Dispatch());
  }

private:
  // matrix expression * vector expression
  static inline
  typename T::NumT 
  Apply(const ArgT& arg, std::size_t i, 
        const Private::TypeOf<Private::row_storage>& LHS_Dispatch,
        const Private::TypeOf<Private::numerical_type>& RHS_Dispatch)
  {
    typedef typename LHS::Disambiguation::RowStorage RowStorage;
    typedef typename LHS::Disambiguation::Allocator MatAllocator;
    typedef typename RowStorage::const_iterator const_iterator;

    typedef typename T::NumT NumT;

//     typedef 
//       MatrixExpression<MatrixDisambiguator<NumT, RowStorage, MatAllocator> >
//       MatExprT;
    

    NumT Result = NumT(0); // Oughta be zero ...
    const RHS& rhs = arg.rhs();

//     // If lhs is matrix, we can use the const reference
//     typedef typename boost::mpl::apply_if_c
//       <
//       SAME_TYPE(),
//       boost::mpl::identity<const RowStorage&>,
//       boost::mpl::identity<RowStorage> 
//       >::type RowType;

    const RowStorage& Row = 
      RowExtractor<typename LHS::Disambiguation>(i)(arg.lhs());

    // for the sake of readability we renounce the use of boost::lambda here and
    // switch back to good ol' STL usage
    const_iterator end = Row.end();
    for (const_iterator iter = Row.begin(); iter != end; ++iter)
      {
        using namespace Daixt::DefaultOps;

        Result += 
          iter->second 
          *
          RowExtractor<VectorExpression<T> >(iter->first)(rhs);
      }
    
    return Result;
  }

  // FIXIT: vector expression * vector expression is handled element-wise, e.g 
  // the result is assumed to be a vector again. Users please do not rely on
  // this feature! It may change in the future.

  // vector expression * scalar value 
  // scalar value * vector expression 
  static inline
  typename T::NumT 
  Apply(const ArgT& arg, std::size_t i, 
        const Private::TypeOf<Private::numerical_type>& LHS_Dispatch,
        const Private::TypeOf<Private::numerical_type>& RHS_Dispatch)
  {
    using namespace Daixt::DefaultOps;

    return
      RowExtractor<VectorExpression<T> >(i)(arg.lhs())
      *
      RowExtractor<VectorExpression<T> >(i)(arg.rhs());      
  }
};



////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// column extractor for matrix expression
////////////////////////////////////////////////////////////////////////////////

template<class T>
struct ColExtractor<MatrixExpression<T> >
{
  ColExtractor(std::size_t i) : i_(i) {}

  template<class ARG> inline
  typename T::RowStorage
  operator()(const ARG& arg) const
  {
    return 
      OperatorDelimImpl<
                        ColExtractor<MatrixExpression<T> >, 
                        typename Daixt::UnwrapExpr<ARG>::Type
                        >::Apply(Daixt::unwrap_expr(arg), i_);
  }

private:
  std::size_t i_;
};


////////////////////////////////////////////////////////////////////////////////
// Matrix
template<class T>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Matrix<typename T::NumT, 
                         typename T::RowStorage,
                         typename T::Allocator> >
{
  static inline
  typename T::RowStorage
  Apply(const Matrix<typename T::NumT, 
                     typename T::RowStorage,
                     typename T::Allocator>& arg,
        std::size_t i) 
  {
    return arg.GetColumn(i);
  }
};


////////////////////////////////////////////////////////////////////////////////
// Daixt::ConstRef
template <class T, class Arg>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::ConstRef<Arg> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::ConstRef<Arg>& arg,
        std::size_t i)
  {
    return ColExtractor<MatrixExpression<T> >(i)(static_cast<const Arg&>(arg));
  }
};


////////////////////////////////////////////////////////////////////////////////
// UnOps

// default: Apply Op to result of ColExtractor(UnOp.arg())
template<class T, class ARG, class Op>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::UnOp<ARG, Op> >
{
  static inline
 
  typename T::RowStorage
  Apply(const Daixt::UnOp<ARG, Op>& arg,
        std::size_t i)
  { 
    typedef typename T::NumT NumT;

    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::iterator iterator;

    RowStorage Col = ColExtractor<MatrixExpression<T> >(i)(arg.arg());

    iterator end = Col.end();
    for (iterator iter = Col.begin(); iter != end; ++iter)
      {
        iter->second = Op::Apply(iter->second, Daixt::Hint<NumT>());
      }

    return Col; 
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOps

// no default, if we use crazy OPs, we need to add further specializations

// matrix expression + matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryPlus>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::iterator iterator;
    
    RowStorage LHS_Result = 
      ColExtractor<MatrixExpression<T> >(i)(arg.lhs());
    RowStorage RHS_Result = 
      ColExtractor<MatrixExpression<T> >(i)(arg.rhs());
    
    return Private::Merge(LHS_Result, RHS_Result);
  }
};


// matrix expression - matrix expression
template<class T, class LHS, class RHS>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<LHS, RHS, Daixt::DefaultOps::BinaryMinus>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::iterator iterator;
    
    RowStorage LHS_Result = 
      ColExtractor<MatrixExpression<T> >(i)(arg.lhs());
    RowStorage RHS_Result = 
      ColExtractor<MatrixExpression<T> >(i)(arg.rhs());
    
    // FIXIT: std::negate kills this when T ain't a builtin type
    return Private::Merge(LHS_Result, RHS_Result, std::negate<NumT>());
  }
};


// FIXIT: missing due to lack of time
// // matrix expression * matrix expression -> some work ;-)


// matrix expression * scalar value
template<class T, class LHS>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<LHS, 
                              Daixt::Scalar<MatrixExpression<T> >, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<LHS, 
                          Daixt::Scalar<MatrixExpression<T> >, 
                          Daixt::DefaultOps::BinaryMultiply>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::value_type Pair;    

    NumT Factor = static_cast<NumT>(arg.rhs());

    typename T::RowStorage Row = 
      ColExtractor<MatrixExpression<T> >(i)(arg.lhs());

    std::for_each
      (Row.begin(), Row.end(), 
       boost::lambda::bind(&Pair::second, boost::lambda::_1) *= Factor);

    return Row;
  }
};


// scalar value * matrix expression
template<class T, class RHS>
struct
OperatorDelimImpl<ColExtractor<MatrixExpression<T> >, 
                  Daixt::BinOp<Daixt::Scalar<MatrixExpression<T> >, 
                              RHS, 
                              Daixt::DefaultOps::BinaryMultiply> >
{
  static inline
  typename T::RowStorage
  Apply(const Daixt::BinOp<Daixt::Scalar<MatrixExpression<T> >, 
                          RHS, 
                          Daixt::DefaultOps::BinaryMultiply>& arg,
        std::size_t i)
  {
    typedef typename T::NumT NumT;
    typedef typename T::RowStorage RowStorage;
    typedef typename RowStorage::value_type Pair;    

    NumT Factor = static_cast<NumT>(arg.lhs());

    typename T::RowStorage Row = 
      ColExtractor<MatrixExpression<T> >(i)(arg.rhs());

    std::for_each
      (Row.begin(), Row.end(), 
       boost::lambda::bind(&Pair::second, boost::lambda::_1) *= Factor);

    return Row;
  }
};


} // namespace Linalg
  

#endif // DAIXT_LINALG_ROW_AND_COLUMN_EXTRACTORS_INC
