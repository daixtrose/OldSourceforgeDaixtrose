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

#ifndef DAIXT_EXPR_INC
#define DAIXT_EXPR_INC

namespace Daixt
{

//////////////////////////////////////////////////////////////////////////////
// Expr inherits from FeaturesOfExpression.
// If You like, You may specialize this class to plug convenient features 
//////////////////////////////////////////////////////////////////////////////

template <class Disambiguation, class T> class FeaturesOfExpression {
public:
  virtual ~FeaturesOfExpression() {};
};


//////////////////////////////////////////////////////////////////////////////
// Expr: A wrapper that makes life a lot easier. 
//////////////////////////////////////////////////////////////////////////////

template <class T>
class Expr 
  :
  public Daixt::FeaturesOfExpression<typename T::Disambiguation, Expr<T> >
{
  T t_;

public:
  typedef typename T::Disambiguation Disambiguation;

  Expr(T t) 
    : t_(t) 
  {} 

  const T& content() const 
  { 
    return t_; 
  }
};

// paranoia: this kind of dirty rubbish ain't allowed!
// => incomplete type to enforce error
template <class T> class Expr<Expr<T> >; 


////////////////////////////////////////////////////////////////////////////////
// Wrap / Unwrap
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Wrap (to hide things)

template <class T>
inline Expr<T> 
make_expr(const T& t) { return Expr<T>(t); }

template <class T>
inline const Expr<T>& 
make_expr(const Expr<T>& ET) { return ET; }


////////////////////////////////////////////////////////////////////////////////
// Unwrap (often it is useful to get rid of the wrapper)

////////////////////////////////////////////////////////////////////////////////
// traits

template <class T>
struct UnwrapExpr { typedef T Type; };

template <class T>
struct UnwrapExpr<Daixt::Expr<T> > { typedef T Type; };


////////////////////////////////////////////////////////////////////////////////
// unwrapper

template <class T>
inline const T& unwrap_expr(const T& t) { return t; } // a do nothing machine


template <class T>
inline T unwrap_expr(const Daixt::Expr<T>& ET) { return ET.content(); } // unwrap

} // namespace Daixt 


#endif // DAIXT_EXPR_INC
