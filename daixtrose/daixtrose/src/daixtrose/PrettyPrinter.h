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

#ifndef DAIXT_PRETTY_PRINTER_INC
#define DAIXT_PRETTY_PRINTER_INC

#include <iostream>

#include "daixtrose/Scalar.h"
#include "daixtrose/NeutralElements.h"
#include "daixtrose/CompileTimeChecks.h"
#include "daixtrose/ChangeDisambiguation.h"


namespace Daixt 
{

namespace OutputUtilities
{

////////////////////////////////////////////////////////////////////////////////
// This file provides 
// std::ostream& operator<<(std::ostream& os, const Daixt::Expr<T>& E)

namespace Private
{

////////////////////////////////////////////////////////////////////////////////
// default formatting. Provides strings for delimiters, etc.

struct DefaultFormatter
{
  typedef const char* String;

  static inline String LeftDelimiter() { return "("; } 
  static inline String RightDelimiter() { return ")"; }
  static inline String One() { return "1"; }
  static inline String Zero() { return "0"; }

  static inline String PowBegin() { return ""; } 
  static inline String PowInter() { return "^"; } 
  static inline String PowEnd() { return ""; } 

  static inline String FractionBegin() { return ""; } 
  static inline String FractionInter() { return " / "; } 
  static inline String FractionEnd() { return ""; } 

  template <class OP> 
  static inline void Print(std::ostream& os) 
  {
    os << " " << OP::Symbol() << " ";  
  }
};



////////////////////////////////////////////////////////////////////////////////
// rudimentary LaTeX support

struct LaTeXFormatter
{
  typedef const char* String;

  static inline String LeftDelimiter() { return "\\left( "; } 
  static inline String RightDelimiter() { return "\\right) "; }
  static inline String One() { return "1 "; }
  static inline String Zero() { return "0 "; }

  static inline String PowBegin() { return "{"; } 
  static inline String PowInter() { return "}^{"; } 
  static inline String PowEnd() { return "} "; } 

  static inline String FractionBegin() { return "\\frac{"; } 
  static inline String FractionInter() { return "}{"; } 
  static inline String FractionEnd() { return "} "; } 

  // "*" is ommitted
  template <class OP> 
  static inline void Print(std::ostream& os) 
  {
    if (SAME_TYPE(OP, Daixt::DefaultOps::BinaryEqual)
        ||
        SAME_TYPE(OP, Daixt::DefaultOps::BinaryModuloAndAssign)) 
      {
        os << " = ";
        return;
      }

    if (!SAME_TYPE(OP, Daixt::DefaultOps::BinaryMultiply)) 
      os << " " << OP::Symbol() << " ";  
  }
};





// default: use operator<<. This yields compiler errors for missing
// specializations or missing operator<<
template <class Formatter, class T> struct PrettyPrinter
{
  static void Print(std::ostream& os, const T& t)
  {
    os << t;// << " ";
  }
};


template <class Formatter, class T> 
struct PrettyPrinter<Formatter, Daixt::ConstRef<T> >
{
  static void Print(std::ostream& os, const Daixt::ConstRef<T>& CR)
  {
    PrettyPrinter<Formatter, T>::Print(os, static_cast<const T&>(CR));
  }
};


template <class Formatter, class T> 
struct PrettyPrinter<Formatter, Daixt::Expr<T> >
{
  static void Print(std::ostream& os, const Daixt::Expr<T>& E)
  {
    PrettyPrinter<Formatter, T>::Print(os, E.content());
  }
};


template <class Formatter, class T, class NewDisambiguation> 
struct PrettyPrinter<Formatter, 
                     Daixt::Expr<Daixt::DisambiguationChanger<T, NewDisambiguation> > >
{
  static void Print(std::ostream& os, 
                    const Daixt::Expr<Daixt::DisambiguationChanger<T, NewDisambiguation> >& E)
  {
    PrettyPrinter<Formatter, T>::Print(os, E.content());
  }
};



template <class Formatter, class Disambiguation> 
struct PrettyPrinter<Formatter, Daixt::Scalar<Disambiguation> >
{
  static void Print(std::ostream& os, const Daixt::Scalar<Disambiguation>& S)
  {
    if (static_cast<typename Daixt::Scalar<Disambiguation>::NumericalType>(S)
        < 
        0) os << Formatter::LeftDelimiter();

//     os << " ";
    os << static_cast<typename Daixt::Scalar<Disambiguation>::NumericalType>(S);
//     os << " ";

    if (static_cast<typename Daixt::Scalar<Disambiguation>::NumericalType>(S)
        < 
        0) os << Formatter::RightDelimiter();
  }
};

template <class Formatter, class Disambiguation> 
struct PrettyPrinter<Formatter, Daixt::IsOne<Disambiguation> >
{
  static void Print(std::ostream& os, const Daixt::IsOne<Disambiguation>& One)
  {
    os << Formatter::One() << " ";
  }
};

template <class Formatter, class Disambiguation> 
struct PrettyPrinter<Formatter, Daixt::IsNull<Disambiguation> >
{
  static void Print(std::ostream& os, const Daixt::IsNull<Disambiguation>& Null)
  {
    os << Formatter::Zero() << " ";
  }
};

////////////////////////////////////////////////////////////////////////////////
// UnOp

template <class Formatter, class ARG, class OP>
struct PrettyPrinter<Formatter, Daixt::UnOp<ARG, OP> >
{
  static void Print(std::ostream& os, const Daixt::UnOp<ARG, OP>& UO)
  {
    static const bool DelimitersAroundArg = 
      FIND(typename Daixt::ExtractOp<ARG>::Result, 
           TYPELIST_4(Daixt::DefaultOps::BinaryPlus,
                      Daixt::DefaultOps::BinaryMinus,
                      Daixt::DefaultOps::BinaryMultiply,
                      Daixt::DefaultOps::BinaryDivide));

    Formatter::template Print<OP>(os);

    if (DelimitersAroundArg) os << Formatter::LeftDelimiter();
    PrettyPrinter<Formatter, ARG>::Print(os, UO.arg());  
    if (DelimitersAroundArg) os << Formatter::RightDelimiter();
  }
};


template <class Formatter, int m, int n, class ARG> 
struct PrettyPrinter<Formatter, 
                     Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<m, n> > >
{
  static void Print
  (std::ostream& os, 
   const Daixt::UnOp<ARG, Daixt::DefaultOps::RationalPower<m, n> >& UO) 
  {
    typedef typename Daixt::ExtractOp<ARG>::Result OP;
    
    static const bool NeedsDelimiters =
      FIND(OP, TYPELIST_5(Daixt::DefaultOps::BinaryPlus,
                          Daixt::DefaultOps::BinaryMinus,
                          Daixt::DefaultOps::BinaryMultiply,
                          Daixt::DefaultOps::BinaryDivide,
                          Daixt::DefaultOps::UnaryMinus));

    os << Formatter::PowBegin();

    if (NeedsDelimiters) os << Formatter::LeftDelimiter();
    PrettyPrinter<Formatter, ARG>::Print(os, UO.arg());
    if (NeedsDelimiters) os << Formatter::RightDelimiter();

    if (n == 1)
      {
        os << Formatter::PowInter();
        (m < 0) ? 
          os << Formatter::LeftDelimiter() << m << Formatter::RightDelimiter() 
          : 
          os << m;
      }
    else
      {
        os << Formatter::PowInter() 
           << Formatter::LeftDelimiter() 
           << Formatter::FractionBegin() 
           << m 
           << Formatter::FractionInter() 
           << n 
           << Formatter::FractionEnd() 
           << Formatter::RightDelimiter();
      }

    os << Formatter::PowEnd();
  }
};


////////////////////////////////////////////////////////////////////////////////
// BinOp

template <class Formatter, class LHS, class RHS, class OP>
struct PrettyPrinter<Formatter, Daixt::BinOp<LHS, RHS, OP> >
{
  static void Print(std::ostream& os, 
                    const Daixt::BinOp<LHS, RHS, OP>& BO) 
  {
    using Daixt::DefaultOps::BinaryMultiply;
    using Daixt::DefaultOps::BinaryDivide;
    using Daixt::DefaultOps::BinaryPlus;
    using Daixt::DefaultOps::BinaryMinus;
    using Daixt::DefaultOps::UnaryMinus;

    static const bool ThisIsMultiplyOrDivide =
      (SAME_TYPE(OP, BinaryMultiply) 
       || 
       SAME_TYPE(OP, BinaryDivide));

    typedef typename Daixt::ExtractOp<LHS>::Result LHS_OP;

    static const bool LHS_NeedsDelimiters =
      (SAME_TYPE(LHS_OP, BinaryPlus)
       ||
       SAME_TYPE(LHS_OP, BinaryMinus)
       ||
       SAME_TYPE(LHS_OP, UnaryMinus));

    typedef typename Daixt::ExtractOp<RHS>::Result  RHS_OP;

    static const bool RHS_NeedsDelimiters =
      (SAME_TYPE(RHS_OP, BinaryPlus)
       ||
       SAME_TYPE(RHS_OP, BinaryMinus)
       ||
       SAME_TYPE(RHS_OP, UnaryMinus));

    static const bool Lhs_delim = 
      ThisIsMultiplyOrDivide && LHS_NeedsDelimiters
      ||
      SAME_TYPE(LHS_OP, UnaryMinus);

    static const bool Rhs_delim = 
      ThisIsMultiplyOrDivide && RHS_NeedsDelimiters
      ||
      SAME_TYPE(RHS_OP, UnaryMinus);
    

    if(SAME_TYPE(OP, BinaryDivide))
      {
        os << Formatter::FractionBegin();
      }

    if (Lhs_delim) os << Formatter::LeftDelimiter();
    PrettyPrinter<Formatter, LHS>::Print(os, BO.lhs());
    if (Lhs_delim) os << Formatter::RightDelimiter();
    
    if(SAME_TYPE(OP, BinaryDivide))
      {
        os << Formatter::FractionInter();
      }
    else
      {
        Formatter::template Print<OP>(os);
      }

    if (Rhs_delim) os << Formatter::LeftDelimiter();
    PrettyPrinter<Formatter, RHS>::Print(os, BO.rhs());
    if (Rhs_delim) os << Formatter::RightDelimiter();

    if(SAME_TYPE(OP, BinaryDivide))
      {
        os << Formatter::FractionEnd();
      }

  }
};


} // namespace Private


////////////////////////////////////////////////////////////////////////////////
// The function we want 
////////////////////////////////////////////////////////////////////////////////

template <class T>
inline std::ostream& operator<<(std::ostream& os, const Daixt::Expr<T>& E)
{
  Private::PrettyPrinter<Private::DefaultFormatter, 
                         Daixt::Expr<T> >::Print(os, E);
  return os;
}


////////////////////////////////////////////////////////////////////////////////
// some magic to achieve correct formatting:
// usage e.g  
// std::cerr << LaTeX() << SomeExpression << std:endl;  

class LaTeXManipulator
{
public:
  inline LaTeXManipulator(std::ostream& os) : os_(os) {}
  inline std::ostream& Os() const { return os_; }

private:
  mutable std::ostream& os_;
};

template <class T>
inline std::ostream& operator<<(const LaTeXManipulator& l, const T& t)
{
  Private::PrettyPrinter<Private::LaTeXFormatter, T>::Print(l.Os(), t);
  return l.Os();
}

struct LaTeX {};

inline LaTeXManipulator operator<<(std::ostream& os, const LaTeX& ignored)
{
  return LaTeXManipulator(os);
}


} // namespace OutputUtilities 

} // namespace Daixt 


#endif // DAIXT_PRETTY_PRINTER_INC
