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
// A _very_ simple example for how to use Daixt.

struct ValueGetter;

class Variable
{
  double Value_;
public:
  Variable() : Value_(0.0) {}
  Variable(double Value) : Value_(Value) {}
  
  template <class T> Variable(const Daixt::Expr<T>& E)
    :
    Value_(ValueGetter()(E)) {} // delegate to the functor

  inline double GetValue() const { return Value_; }
  inline void SetValue(double Value) { Value_ = Value; }
};

////////////////////////////////////////////////////////////////////////////////
// all overloads of a feature are gathered together in one place, the functor
// ValueGetter. You could have the same effect with free functions, too
struct ValueGetter {
  inline double operator()(const Variable& v) const 
  { 
    return v.GetValue(); 
  }

  template<class T> 
  double operator()(const Daixt::Expr<T>& E) const 
  { 
    return (*this)(E.content()); 
  }

  //////////////////////////////////////////////////////////////////////////////
  // always rely on the default behaviour

  template <class ARG, class OP>
  double 
  operator()(const Daixt::UnOp<ARG, OP>& UO) const  
  {
    return OP::Apply((*this)(UO.arg()), 
                     Daixt::Hint<double>());
  }

  template <class LHS, class RHS, class OP>
  double 
  operator()(const Daixt::BinOp<LHS, RHS, OP>& BO) const  
  {
    return OP::Apply((*this)(BO.lhs()), 
                     (*this)(BO.rhs()), 
                     Daixt::Hint<double>());
  }
};


////////////////////////////////////////////////////////////////////////////////
// Introduce some features we would like to add sometimes to the expression

struct SomeFeature 
{
  virtual double GetValue() const = 0;
  virtual SomeFeature* DeepCopy() const = 0;
  virtual ~SomeFeature() {}
};



namespace Daixt
{
template <class T> 
class FeaturesOfExpression<SomeFeature, T>
  : public SomeFeature
{
public:
  double GetValue() const
  {
    return ValueGetter()(static_cast<const T&>(*this));
  }
  
  FeaturesOfExpression<SomeFeature, T>* DeepCopy() const 
  {
    return new T(static_cast<const T&>(*this)); 
  }
  
  virtual ~FeaturesOfExpression() {};
};

} // namespace Daixt

//////////////////////////////////////////////////////////////////////////////

int main()
{
  using namespace Daixt::DefaultOps;

  Variable a = 5.0, b = 3.0, c = 2.0;
  Variable d = a + b + c / (a - b);

  std::cerr << ValueGetter()(d) << std::endl;

  std::cerr << Daixt::ChangeDisambiguation<SomeFeature>
    (a + b + c / (a - b)).GetValue() << '\n';

  SomeFeature* sf = 
    Daixt::ChangeDisambiguation<SomeFeature>(a + b + c / (a - b)).DeepCopy();
  
  std::cerr << sf->GetValue() << std::endl;
}

