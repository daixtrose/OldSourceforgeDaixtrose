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

#include <iostream>

#include "daixtrose/Daixt.h"

////////////////////////////////////////////////////////////////////////////////
// A _very_ simple example for how to use Daixt.  Extended by delegation inside
// ValueGetter

struct ValueGetter;

class Variable
{
  double Value_;
public:
  Variable() : Value_(0.0) {}
  Variable(double Value) : Value_(Value) {}
  
  template <class T> Variable(const Daixt::Expr<T>& E);

  inline double GetValue() const { return Value_; }
  inline void SetValue(double Value) { Value_ = Value; }
};


////////////////////////////////////////////////////////////////////////////////
// all overloads of a feature are reached through a forwarding functor
// ValueGetter. You cannot have the same effect with free functions!  This
// enables user extensibility via specialization and - in contrast to free
// functions - allows partial template specialization (!)

// class ReturnType is given as dummy argument in order to ensure _any_
// specialization of OperatorDelimImpl is only _partially_ specialized.  So we
// do not need to care about point of diagnostics and order issues, since all
// diagnostics are deferred until instantiation.
template<class ReturnType, class T, class ARG> struct OperatorDelimImpl;

struct ValueGetter {
  template<class ARG> 
  inline double operator()(const ARG& arg) const 
  { 
    return OperatorDelimImpl<double, ValueGetter, ARG>::Apply(arg); 
  }
};


template<class ReturnType> 
struct OperatorDelimImpl<ReturnType, ValueGetter, Variable>
{
  static inline ReturnType Apply(const Variable& v) { return v.GetValue(); }
};


template<class ReturnType, class T> 
struct OperatorDelimImpl<ReturnType, ValueGetter, Daixt::Expr<T> >
{
  static inline ReturnType Apply(const Daixt::Expr<T>& E) 
  { 
    return ValueGetter()(E.content()); 
  }
};

////////////////////////////////////////////////////////////////////////////////
// always rely on the default behaviour

// UnOp
template<class ReturnType, class ARG, class OP>
struct OperatorDelimImpl<ReturnType, ValueGetter, Daixt::UnOp<ARG, OP> >
{
  static inline ReturnType Apply(const Daixt::UnOp<ARG, OP>& UO)  
  {
    return OP::Apply(ValueGetter()(UO.arg()), 
                     Daixt::Hint<double>());
  }
};

// BinOp
template <class ReturnType, class LHS, class RHS, class OP>
struct OperatorDelimImpl<ReturnType, ValueGetter, Daixt::BinOp<LHS, RHS, OP> >
{
  static inline ReturnType Apply(const Daixt::BinOp<LHS, RHS, OP>& BO)  
  {
    return OP::Apply(ValueGetter()(BO.lhs()), 
                     ValueGetter()(BO.rhs()), 
                     Daixt::Hint<double>());
  }
};


////////////////////////////////////////////////////////////////////////////////
// Due to a "defect" of the standard one has to postpone the definition of
// this contructor until here

template <class T>  Variable::Variable(const Daixt::Expr<T>& E)
  :
  Value_(ValueGetter()(E)) {} // delegate to the functor



int main()
{
  using namespace Daixt::DefaultOps;

  Variable a = 5.0, b = 3.0, c = 1.99;
  Variable d = - (a + b + c / (a - b));

  std::cerr << ValueGetter()(d) << std::endl;
}

