//-*-C++-*- 
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

#ifndef DAIXT_LINALG_PRINT_BLOCKED_MATRIX_INC
#define DAIXT_LINALG_PRINT_BLOCKED_MATRIX_INC


#include "daixtrose/Daixt.h"

#include "linalg/Matrix.h"
#include "tiny/TinyMatrix.h"
#include "tiny/TinyVector.h"

#include "boost/utility.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>


namespace Linalg
{

////////////////////////////////////////////////////////////////////////////////
// Output utility: treat a Matrix of TinyMatrices in a special way 
// i.e. human-readable blockwise output with row and column separators


template <class Double, int N, class RowStorage>
std::ostream& operator<<
  (std::ostream& os, 
   const Linalg::Matrix<TinyMat::TinyQuadraticMatrix<Double, N>, RowStorage>& M)
{
  if (M.nrows() == 0)
    throw std::range_error
      ("operator<<(..., LinalgMatrix<TinyQuadraticMatrix<...>>:"
       " number of rows == 0");

  if (M.ncols() == 0)
    throw std::range_error
      ("operator<<(..., LinalgMatrix<TinyQuadraticMatrix<...>>:"
       " number of cols == 0");


  // short forms
  typedef TinyMat::TinyQuadraticMatrix<Double, N> BlockMatrix;
  typedef Linalg::Matrix<BlockMatrix, RowStorage> Matrix;
  
  // layout constants
  const std::size_t LengthOfValue = 12;
  const std::size_t LengthOfBlockRow = (LengthOfValue+1) * N;
  const std::string BlockSeparator = " | ";
  
  std::vector<char> Emptyness(LengthOfBlockRow, ' ');
  // dashes
  for (std::size_t i = 0; i != N; ++i)
    {
      Emptyness[i * LengthOfValue + LengthOfValue - 1] = '-';
    }
  
  // A first row separator
  for (std::size_t i = 1; 
       i != (Emptyness.size() + BlockSeparator.size()) * M.ncols(); ++i)
    {
      os << '-';
    }
  os << '\n';
  
  // now row by row to os
  typedef TinyVec::TinyVector<std::ostringstream, N> StreamVector;
  
  std::size_t nmax = M.nrows() + 1;
  for (std::size_t n = 1; n != nmax; ++n)
    {
      const RowStorage & Row = M(n);

      StreamVector Streams;

      typedef typename RowStorage::const_iterator const_iterator;
      const_iterator end = Row.end();
      const_iterator begin = Row.begin();
      
      std::size_t NumberOfPrintedBlocks = 0;

      for (const_iterator iter = begin; iter != end; ++iter)
        {
          // until no more empty entries
          std::size_t NumberOfEmptyBlocks =
            iter == begin?
            iter->first - 1 :
            iter->first - boost::prior(iter)->first - 1;
            
          NumberOfPrintedBlocks += NumberOfEmptyBlocks;

          for (size_t k = 0; k != NumberOfEmptyBlocks; ++k)
            {
              for (std::size_t i = 1; i != N + 1; ++i)
                {
                  std::copy(Emptyness.begin(), Emptyness.end(),
                            std::ostream_iterator<char>(Streams(i)));
                  
                  Streams(i) << BlockSeparator;
                }
            }

          ++NumberOfPrintedBlocks;
          for (std::size_t i = 1; i != N + 1; ++i)
            {
              for (size_t j = 1; j != N + 1; ++j)
                {
                  Streams(i) << std::setw(LengthOfValue) << (iter->second)(i, j) << ' ';
                }
              Streams(i) << BlockSeparator;
            }
        }

      // trailing emptyness (no dashes)
      std::vector<char> TrailingEmptyness(LengthOfBlockRow, ' ');

      std::size_t NumberOfEmptyBlocks = M.ncols() - NumberOfPrintedBlocks;
      for (size_t k = 0; k != NumberOfEmptyBlocks; ++k)
        {
          for (std::size_t i = 1; i != N + 1; ++i)
            {
              std::copy(TrailingEmptyness.begin(), TrailingEmptyness.end(),
                        std::ostream_iterator<char>(Streams(i)));
              
              Streams(i) << BlockSeparator;
            }
        }

      // output of rows:
      for (std::size_t i = 1; i != N + 1; ++i)
        {
          os << Streams(i).str() << std::endl;
        }
      

      // row separator
      for (std::size_t i = 1; 
           i != (Emptyness.size() + BlockSeparator.size()) * M.ncols(); ++i)
        {
          os << '-';
        }
      os << std::endl;
    }

  return os;
}



////////////////////////////////////////////////////////////////////////////////
// Same for Matrix of BlockVectors

template <class Double, int N, class RowStorage>
std::ostream& operator<<
  (std::ostream& os, 
   const Linalg::Matrix<TinyVec::TinyVector<Double, N>, RowStorage>& M)
{
  if (M.nrows() == 0)
    throw std::range_error
      ("operator<<(..., LinalgMatrix<TinyQuadraticMatrix<...>>:"
       " number of rows == 0");

  if (M.ncols() == 0)
    throw std::range_error
      ("operator<<(..., LinalgMatrix<TinyQuadraticMatrix<...>>:"
       " number of cols == 0");


  // short forms
  typedef TinyVec::TinyVector<Double, N> BlockVector;
  typedef Linalg::Matrix<BlockVector, RowStorage> Matrix;
  
  // layout constants
  const std::size_t LengthOfValue = 12;
  const std::size_t LengthOfBlockRow = (LengthOfValue+1);
  const std::string BlockSeparator = " | ";
  
  std::vector<char> Emptyness(LengthOfBlockRow, ' ');
  // dash
  Emptyness[LengthOfValue - 1] = '-';
  
  
  // A first row separator
  for (std::size_t i = 1; 
       i != (Emptyness.size() + BlockSeparator.size()) * M.ncols(); ++i)
    {
      os << '-';
    }
  os << '\n';
  
  // now row by row to os
  typedef TinyVec::TinyVector<std::ostringstream, N> StreamVector;
  
  std::size_t nmax = M.nrows() + 1;
  for (std::size_t n = 1; n != nmax; ++n)
    {
      const RowStorage & Row = M(n);

      StreamVector Streams;

      typedef typename RowStorage::const_iterator const_iterator;
      const_iterator end = Row.end();
      const_iterator begin = Row.begin();
      
      std::size_t NumberOfPrintedBlocks = 0;

      for (const_iterator iter = begin; iter != end; ++iter)
        {
          // until no more empty entries
          std::size_t NumberOfEmptyBlocks =
            iter == begin?
            iter->first - 1 :
            iter->first - boost::prior(iter)->first - 1;
            
          NumberOfPrintedBlocks += NumberOfEmptyBlocks;

          for (size_t k = 0; k != NumberOfEmptyBlocks; ++k)
            {
              for (std::size_t i = 1; i != N + 1; ++i)
                {
                  std::copy(Emptyness.begin(), Emptyness.end(),
                            std::ostream_iterator<char>(Streams(i)));
                  
                  Streams(i) << BlockSeparator;
                }
            }

          ++NumberOfPrintedBlocks;
          for (std::size_t i = 1; i != N + 1; ++i)
            {
              Streams(i) << std::setw(LengthOfValue) << (iter->second)(i) << ' ';
              Streams(i) << BlockSeparator;
            }
        }

      // trailing emptyness (no dashes)
      std::vector<char> TrailingEmptyness(LengthOfBlockRow, ' ');

      std::size_t NumberOfEmptyBlocks = M.ncols() - NumberOfPrintedBlocks;
      for (size_t k = 0; k != NumberOfEmptyBlocks; ++k)
        {
          for (std::size_t i = 1; i != N + 1; ++i)
            {
              std::copy(TrailingEmptyness.begin(), TrailingEmptyness.end(),
                        std::ostream_iterator<char>(Streams(i)));
              
              Streams(i) << BlockSeparator;
            }
        }

      // output of rows:
      for (std::size_t i = 1; i != N + 1; ++i)
        {
          os << Streams(i).str() << std::endl;
        }
      

      // row separator
      for (std::size_t i = 1; 
           i != (Emptyness.size() + BlockSeparator.size()) * M.ncols(); ++i)
        {
          os << '-';
        }
      os << std::endl;
    }

  return os;
}



  
}

#endif // DAIXT_LINALG_PRINT_BLOCKED_MATRIX_INC
