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

#ifndef DAIXT_CHANGE_DISAMBIGUATION_INC
#define DAIXT_CHANGE_DISAMBIGUATION_INC

#include "daixtrose/Expr.h"

////////////////////////////////////////////////////////////////////////////////
// sometimes an expression must move its disambiguation in order to obtain
// access to certain features (Expr inherits from FeaturesOfExpression)
// This "Function" provides a convenient way to "force" all disambiguations 
// to change except for the leaves 

namespace Daixt 
{

template <class T, class NewDisambiguation>
class DisambiguationChanger
{
  T t_;

public:
  typedef NewDisambiguation Disambiguation;

  DisambiguationChanger(Daixt::Expr<T> E) 
    : t_(E.content()) 
  {} 

  T content() const 
  { 
    return t_; 
  }
};  


template <class T, class NewDisambiguation>
class Expr<DisambiguationChanger<T, NewDisambiguation> > 
  :
    public Daixt::FeaturesOfExpression<typename NewDisambiguation, 
                                       Expr<DisambiguationChanger<T, NewDisambiguation> > >
{
  T t_;

public:
  typedef typename T::Disambiguation Disambiguation;

  Expr(DisambiguationChanger<T, NewDisambiguation> DCT) 
    : t_(DCT.content()) 
  {} 

  T content() const 
  { 
    return t_; 
  }
};


template <class NewDisambiguation, class T>
class Expr<DisambiguationChanger<T, NewDisambiguation> >
ChangeDisambiguation(Expr<T> E)
{
  typedef Expr<DisambiguationChanger<T, NewDisambiguation> > ResultT;
  return  ResultT(DisambiguationChanger<T, NewDisambiguation>(E));
}

} // namespace Daixt 

#endif // DAIXT_CHANGE_DISAMBIGUATION_INC
