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
// developers of daixt (see e.g. http://daixt.sourceforge.net/)

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU Lesser General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU Lesser General Public License.

#ifndef DAIXT_BINOPS_INC
#define DAIXT_BINOPS_INC


#include "daixtrose/Disambiguation.h"
#include "daixtrose/CompileTimeChecks.h"

#include "boost/mpl/apply_if.hpp"
#include "boost/mpl/identity.hpp"


////////////////////////////////////////////////////////////////////////////////
// Binary Operations
////////////////////////////////////////////////////////////////////////////////

namespace Daixt 
{

template <class LHS, class RHS, class OP>
class BinOp
{
private:
  LHS lhs_;
  RHS rhs_;

public:
  explicit inline BinOp(const LHS& lhs, const RHS& rhs) 
    : lhs_(lhs), 
      rhs_(rhs) 
  {} 

  inline LHS lhs() const { return lhs_; }
  inline RHS rhs() const { return rhs_; }

private:
  // disambiguation mechanism: if *HS does not contain a typedef or member named
  // "Disambiguation", the disambiguation type is set to *HS itself

  typedef typename boost::mpl::apply_if
  <
    Check::has_disambiguation<LHS>,
    Check::use_disambiguation<LHS>,
    boost::mpl::identity<LHS> 
  >::type LHS_Disambiguation;

  typedef typename boost::mpl::apply_if
  <
    Check::has_disambiguation<RHS>,
    Check::use_disambiguation<RHS>,
    boost::mpl::identity<RHS> 
  >::type RHS_Disambiguation;

public:
  typedef typename BinOpResultDisambiguator<LHS_Disambiguation, 
                                            RHS_Disambiguation, 
                                            OP>::Disambiguation Disambiguation;
};


} // namespace Daixt


#endif // DAIXT_BINOPS_INC
