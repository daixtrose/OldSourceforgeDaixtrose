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

#ifndef DAIXT_MATRIX_SELECT_INC
#define DAIXT_MATRIX_SELECT_INC

#include "boost/mpl/apply_if.hpp"
#include "boost/mpl/identity.hpp"

////////////////////////////////////////////////////////////////////////////////
// MatrixSelect is a multidimensional decision taker
// This allows for AND or OR relations to hold 
////////////////////////////////////////////////////////////////////////////////

namespace Daixt
{

using namespace boost::mpl;

template <bool B1, bool B2,
          class ff, class tf,
          class ft, class tt>
struct MatrixSelect2
{
  typedef typename
  apply_if_c<
    B1, 
    if_c<B2, tt, tf>, 
    if_c<B2, ft, ff>
  >::type type;
};


} // namespace Daixt


#endif // DAIXT_MATRIX_SELECT_INC
