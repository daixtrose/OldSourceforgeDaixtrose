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
// developers of daixtrose (see e.g. http://daixtrose.sourceforge.net/)

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU Lesser General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU Lesser General Public License.

#ifndef DAIXT_LINALG_MATRIX_INC
#define DAIXT_LINALG_MATRIX_INC


#include "daixtrose/Daixt.h"

#include "linalg/RowAndColumCounters.h"
#include "linalg/RowAndColumExtractors.h"
#include "linalg/Disambiguation.h"


#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lexical_cast.hpp"


#include <vector>
#include <cassert>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <iosfwd>
#include <iomanip>


////////////////////////////////////////////////////////////////////////////////
// An STL-based sparse matrix implementation
////////////////////////////////////////////////////////////////////////////////

namespace Linalg
{

// implemented in other file
template <class T> inline size_t NumberOfRows(const T& t);
template <class T> inline size_t NumberOfCols(const T& t);


namespace Private
{

////////////////////////////////////////////////////////////////////////////////
// This is an SGI STL extension included for portability
// I put it in namespace Private so that it won't interfere with any others
template <class Pair>
struct select1st 
  : public std::unary_function<Pair, typename Pair::first_type> 
{
  inline typename Pair::first_type& operator()(Pair& p) const 
  {
    return p.first;
  }

  inline const typename Pair::first_type& operator()(const Pair& p) const 
  {
    return p.first;
  }
};

} // namespace Private


////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////

template <class T, class RowStorageT, class AllocatorT>
struct MatrixDisambiguator
{
  typedef T NumT;
  typedef RowStorageT RowStorage;
  typedef AllocatorT Allocator;
}; 


template <
          class T, // numerical type
          // must have the same interface and semantics as std::map<size_t, T>
          class RowStorage = std::map<std::size_t, 
                                      T, 
                                      std::less<std::size_t>,
                                      std::allocator<std::pair<const std::size_t, T> > >,
          class Allocator = std::allocator<RowStorage> 
          >
class Matrix
{
public:
  // disambiguation
  typedef MatrixExpression<MatrixDisambiguator<T, RowStorage, Allocator> > 
  Disambiguation;

  typedef Matrix<T, RowStorage, Allocator> MyOwnType;
  // this design decision (std::vector) could be replaced by a template
  // argument, but std::vector does a fairly good job here.  We assume C++
  // standard is fixed to guarantee the data is stored continously
  typedef std::vector<RowStorage, Allocator> DataStorageT;

  typedef RowStorage RowStorageT;
  typedef Allocator AllocatorT;

  //////////////////////////////////////////////////////////////////////////////

  inline Matrix(size_t rows = 0, size_t cols = 0);

  inline Matrix(const MyOwnType& Other);

  template<class OtherT> inline Matrix(const OtherT& Other);


  //////////////////////////////////////////////////////////////////////////////

  inline MyOwnType& operator=(const MyOwnType& Other);

  // this one retains sparsity pattern
  inline MyOwnType& operator=(const T& t);

  // assignments which use RowExtractor
  template<class OtherT> 
  inline MyOwnType& operator=(const OtherT& Other);

  template<class OtherT> 
  inline MyOwnType& operator+=(const OtherT& Other);

  template<class OtherT>
  inline MyOwnType& operator-=(const OtherT& Other);


  inline void swap(MyOwnType& Other);


  inline ~Matrix();

  inline size_t nrows() const { return nrows_; }
  inline size_t ncols() const { return ncols_; }

  inline T& operator()(size_t i, size_t j);
  inline const T& operator()(size_t i, size_t j) const;

  //////////////////////////////////////////////////////////////////////////////
  // for sparse matrices it is fine to have access to single rows
  // this again is fattening the interface, but numerics require maximum
  // efficiency, so we give up the political correct OOP information hiding 

  // We always return const ref here. This ensures the column info is valid all
  // times:  If You need to modify a row, You must use ReplaceRow from below,
  // which updates ColumnInfo correctly
  inline const RowStorage& operator()(size_t i) const;

  std::vector<size_t> NumberOfEntriesPerRow();

  // according to Herb Sutter these should reside outside the class as friend
  // functions. Herb Sutter is right (as always), but I see no advantage here,
  // neither in maintainability nor in performance, so they stay here until
  // otherwise convinced.
  template <class Val>
  inline void SetEntriesInRowTo(size_t i, Val val);

  template <class Val>
  inline void SetEntriesInColTo(size_t i, Val val);

  inline void ReplaceRow(size_t i, RowStorage Row); 

  // yes, sometimes we cannot avoid access to columns
  inline RowStorage GetColumn(size_t j) const;

private:
  size_t nrows_;
  size_t ncols_;
  
  DataStorageT data_;

  inline void RangeCheck(size_t i, size_t j) const; 
  inline const T& Zero() const;


  // Dealing with colums in a sparse context needs some extra bookkeeping.  Like
  // this all read accesses and the write acceses to existing entries gain
  // speed, while write accesses to non-existent entries slow down a bit. Of
  // course memory consumption is higher, but this solution IMHO is the winner.
  // Your mileage may vary. 

  typedef std::vector<size_t> IndexStorage;
  typedef std::vector<IndexStorage> ColumnInfoStorage;

  ColumnInfoStorage ColumnInfo_;

  inline void UpdateColumnInfo(size_t row_index, 
                               const RowStorage& OldRow,
                               const RowStorage& NewRow);

};


////////////////////////////////////////////////////////////////////////////////
// data output
template<class T, class RowStorage, class Allocator> 
inline 
std::ostream& operator<< (std::ostream& o, 
                          const Matrix<T, RowStorage, Allocator>& M);


////////////////////////////////////////////////////////////////////////////////
// **************************** IMPLEMENTATION *******************************//
////////////////////////////////////////////////////////////////////////////////

template<class T, class RowStorage, class Allocator>
Matrix<T, RowStorage, Allocator>::
Matrix(size_t rows, size_t cols) 
  : 
  nrows_(rows), 
  ncols_(cols), 
  data_(nrows_), // yes, default constructor of RowStorages oughta be called 
  ColumnInfo_(ncols_) // similar here
{
  // Incomplete implementation: at present the whole matrix lib is only approved
  // and written for quadratic matrices with generalization postponed due to
  // lack of time.  Should be some more elaborate error message.
  assert(rows == cols); // sorry, not implemented yet!
}


template<class T, class RowStorage, class Allocator>
Matrix<T, RowStorage, Allocator>::
Matrix(const Matrix<T, RowStorage, Allocator>& Other)
  :
  nrows_(Other.nrows()), 
  ncols_(Other.ncols()), 
  data_(Other.data_), 
  ColumnInfo_(Other.ColumnInfo_)
{}


template<class T, class RowStorage, class Allocator>
template<class OtherT>
Matrix<T, RowStorage, Allocator>::
Matrix(const OtherT& Other) 
  :
  nrows_(NumberOfRows(Other)),
  ncols_(NumberOfCols(Other)),
  data_(),
  ColumnInfo_(ncols_) // must be initialized
{
  data_.reserve(nrows_);

  RowStorage Empty;

  for (size_t i = 1; i != nrows_ + 1; ++i)
    {
      data_.push_back(RowExtractor<Disambiguation>(i)(Other));
      UpdateColumnInfo(i, Empty, data_[i-1]);
    }
}


template<class T, class RowStorage, class Allocator>
Matrix<T, RowStorage, Allocator>& 
Matrix<T, RowStorage, Allocator>::
operator=(const Matrix<T, RowStorage, Allocator>& Other)
{
  if (this != &Other)
    {
      // FIXIT: exception safety issues completely ignored here (shame on me)
      nrows_ = Other.nrows(); 
      ncols_ = Other.ncols();
      
      data_ = Other.data_; 
      ColumnInfo_ = Other.ColumnInfo_;
    }
  return *this;
}


template<class T, class RowStorage, class Allocator>
Matrix<T, RowStorage, Allocator>& 
Matrix<T, RowStorage, Allocator>::
operator=(const T& t)
{
  typename DataStorageT::iterator end = data_.end();
  for (typename DataStorageT::iterator iter = data_.begin(); iter != end; ++iter)
    {
      typedef typename RowStorage::iterator iterator;
      iterator rsend = iter->end();
      for (iterator rsiter = iter->begin(); rsiter != rsend; ++rsiter)
        {
          rsiter->second = t; 
        }
    }
  
  return *this;
}


template<class T, class RowStorage, class Allocator>
template<class OtherT>
Matrix<T, RowStorage, Allocator>& 
Matrix<T, RowStorage, Allocator>::
operator=(const OtherT& Other)
{
  // FIXIT: We do not check the indices of the result of GetRow(Other)
  // so if someone makes stupid things there, we have an undetected error here.

  // The check for whether we need a temporary could be refined: if *this does
  // not occur on the rhs of a multiplication, the temporary might not be
  // required.
  if (Daixt::CountOccurrence(Other, *this)) // must use a temporary
    {
      size_t nrows = NumberOfRows(Other);
      size_t ncols = NumberOfCols(Other);

      MyOwnType Tmp(nrows, ncols);
      
      for (size_t i = 1; i != nrows_ + 1; ++i)
        {
          Tmp.ReplaceRow(i, RowExtractor<Disambiguation>(i)(Other));
        }
      
      this->swap(Tmp);
    }
  else // Not so many temporaries needed
    {
      size_t nrows = NumberOfRows(Other);
      size_t ncols = NumberOfCols(Other);

      // tabula rasa when sizes do not fit
      if ((nrows_ != nrows) || (ncols_ != ncols))
        {
          nrows_ = nrows;
          ncols_ = ncols;

          std::for_each(ColumnInfo_.begin(), ColumnInfo_.end(),
                        std::mem_fun_ref(&IndexStorage::clear));
          
          ColumnInfo_.resize(nrows_); 
          
          std::for_each(data_.begin(), data_.end(),
                        std::mem_fun_ref(&RowStorage::clear));
          
          data_.resize(nrows_);
        }

      // assing row by row
      for (size_t i = 1; i != nrows_ + 1; ++i)
        {
          RowStorage& ActualRow = data_[i-1];
          
          RowStorage NewRow = RowExtractor<Disambiguation>(i)(Other);
          
          UpdateColumnInfo(i, ActualRow, NewRow);
          ActualRow.swap(NewRow);
        }
    }
  
  return *this;
}


template<class T, class RowStorage, class Allocator>
template<class OtherT>
Matrix<T, RowStorage, Allocator>& 
Matrix<T, RowStorage, Allocator>::
operator+=(const OtherT& Other)
{
  size_t nrows = NumberOfRows(Other);
  size_t ncols = NumberOfCols(Other);

#ifndef NDEBUG
  if (nrows_ != nrows) 
    throw 
      std::range_error("Matrix<T, RowStorage, Allocator>::operator+=: "
                       "the number of rows is incompatible");
  if (ncols_ != ncols)
    throw 
      std::range_error("Matrix<T, RowStorage, Allocator>::operator+=: "
                       "the number of columns is incompatible");
#endif

  if (Daixt::CountOccurrence(Other, *this)) 
    {
      //using Daixt::DefaultOps::operator+; // gcc parser bug
      using namespace Daixt::DefaultOps;
      // delegate to operator= which uses temporary
      // GCC-3.3-cvs is stupid so we give it explicitly
      *this = Daixt::DefaultOps::operator+ <MyOwnType, MyOwnType>
        (static_cast<const MyOwnType&>(*this), Other); 
    }
  else
    {
      for (size_t i = 1; i != nrows_ + 1; ++i)
        {
          RowStorage Row = RowExtractor<Disambiguation>(i)(Other);
          RowStorage& MyRow = data_[i-1];

          // merge Row with MyRow
          typedef typename RowStorage::const_iterator const_iterator;
          typedef typename RowStorage::iterator iterator;

          const_iterator end = Row.end();
          for (const_iterator iter = Row.begin(); iter!= end; ++iter) 
            {
              const std::size_t j = iter->first;

              iterator lb = MyRow.lower_bound(j);
              // if the entry already exists  ...
              if(lb != MyRow.end() && !(MyRow.key_comp()(j, lb->first))) 
                {
                  lb->second += iter->second;
                } 
              else 
                {
                  ColumnInfo_[j-1].push_back(i);
                  MyRow.insert(lb, *iter);
                }
            }
        }
    }

  return *this;
}


template<class T, class RowStorage, class Allocator>
template<class OtherT>
Matrix<T, RowStorage, Allocator>& 
Matrix<T, RowStorage, Allocator>::
operator-=(const OtherT& Other)
{
  // delegate to +=
  //using Daixt::DefaultOps::operator-;
  using namespace Daixt::DefaultOps;
  return this->operator+=(Daixt::ExprManip::Simplify(- Other));
}


template<class T, class RowStorage, class Allocator>
void
Matrix<T, RowStorage, Allocator>::
swap(Matrix<T, RowStorage, Allocator>& Other)
{
  // FIXIT: exception safety issues completely ignored here (shame on me)
  std::swap(this->nrows_, Other.nrows_);
  std::swap(this->ncols_, Other.ncols_);
  this->data_.swap(Other.data_);
  this->ColumnInfo_.swap(Other.ColumnInfo_);
}


template<class T, class RowStorage, class Allocator> 
Matrix<T, RowStorage, Allocator>::
~Matrix() {}


template<class T, class RowStorage, class Allocator>
T&
Matrix<T, RowStorage, Allocator>::
operator()(size_t i, size_t j) 
{
  RangeCheck(i, j);

  RowStorage& Row = data_[i-1];

  typedef typename RowStorage::iterator iterator;
  iterator lb = Row.lower_bound(j);

  // if the entry already exists  ...
  if(lb != Row.end() && !(Row.key_comp()(j, lb->first))) 
    {
      // ... return a reference to the value
      return lb->second;
    } 
  else 
    { 
      // update the column info
      ColumnInfo_[j-1].push_back(i);

      // insert new data and return ref to it
      typedef typename RowStorage::value_type MVT;
      return Row.insert(lb, MVT(j, T(0)))->second;
    }
} 
  

template<class T, class RowStorage, class Allocator>
const T&
Matrix<T, RowStorage, Allocator>::
operator()(size_t i, size_t j) const 
{
  RangeCheck(i, j);

  const RowStorage& Row = data_[i-1];

  typedef typename RowStorage::const_iterator const_iterator;
  const_iterator lb = Row.lower_bound(j);

  // if the entry already exists  ...
  if(lb != Row.end() && !(Row.key_comp()(j, lb->first))) 
    {
      // .. we return a const ref to it
      return lb->second;
    } 
  else 
    { 
      // we inhibit insertion into the row to keep sparsity
      return this->Zero();
    }
}


template<class T, class RowStorage, class Allocator>
const RowStorage&
Matrix<T, RowStorage, Allocator>::
operator()(size_t i) const 
{
  RangeCheck(i, 1);
  return data_[i-1];
}


template<class T, class RowStorage, class Allocator>
template <class Val>
void 
Matrix<T, RowStorage, Allocator>::
SetEntriesInRowTo(size_t i, Val val)
{
  RangeCheck(i, 1);

  RowStorage& Row = data_[i-1];
  typedef typename RowStorage::iterator iterator;

  iterator end = Row.end();
  for (iterator iter = Row.begin(); iter!= end; ++iter) 
    {
      iter->second = val;
    }
//   using boost::lambda::_1;

//   std::for_each(data_[i-1].begin(), data_[i-1].end(), 
//                 (&_1 ->* &RowStorage::value_type::second) = val);
}


template<class T, class RowStorage, class Allocator>
template <class Val>
void 
Matrix<T, RowStorage, Allocator>::
SetEntriesInColTo(size_t j, Val val)
{
  RangeCheck(1, j);

  const IndexStorage& RowNumbers = ColumnInfo_[j-1];

  typedef typename IndexStorage::const_iterator iterator;

  iterator end = RowNumbers.end();
  for (iterator iter = RowNumbers.begin(); iter != end; ++iter)
    {
      RowStorage& Row = data_[*iter-1]; 

      typedef typename RowStorage::iterator iterator;      
      iterator entry = Row.lower_bound(j);

      if (entry != Row.end() && !(Row.key_comp()(j, entry->first)))
        {
          entry->second = val; 
        }
      else
        {
          throw 
            std::logic_error
            ("Fatal Error: Inconsistency in Matrix<T, RowStorage, Allocator>::"
             "ColumnInfo detected.\n"
             "This is a serious bug, please send a bug report to the "
             "authors of this library.\n");
        }
    }
}


template<class T, class RowStorage, class Allocator>
void 
Matrix<T, RowStorage, Allocator>::
ReplaceRow(size_t i, RowStorage Row)
{
  UpdateColumnInfo(i, data_[i-1], Row);
  data_[i-1].swap(Row);
}


template<class T, class RowStorage, class Allocator>
std::vector<size_t> 
Matrix<T, RowStorage, Allocator>::
NumberOfEntriesPerRow() 
{
  std::vector<size_t> Result;
  Result.reserve(nrows_);

  for (size_t i = 0; i < nrows_; ++i) 
    {
      Result.push_back(data_[i].size());
    }

  return Result;
}


template<class T, class RowStorage, class Allocator>
RowStorage
Matrix<T, RowStorage, Allocator>::
GetColumn(size_t j) const
{
  RangeCheck(1, j);
  const IndexStorage& RowNumbers = ColumnInfo_[j-1];

  RowStorage Result;

  typedef typename IndexStorage::const_iterator iterator;

  iterator end = RowNumbers.end();
  for (iterator iter = RowNumbers.begin(); iter != end; ++iter)
    {
      const RowStorage& Row = data_[*iter-1]; 

      typedef typename RowStorage::const_iterator iterator;      
      iterator entry = Row.lower_bound(j);

      if (entry != Row.end() && !(Row.key_comp()(j, entry->first)))
        {
          typedef typename RowStorage::value_type value_type;
          Result.insert(value_type(*iter, entry->second)); 
        }
      else
        {
          throw 
            std::logic_error
            ("Fatal Error: Inconsistency in Matrix<T, RowStorage, Allocator>::"
             "ColumnInfo detected.\n"
             "This is a serious bug, please send a bug report to the "
             "authors of this library.\n");
        }
    }

  return Result;
}


template<class T, class RowStorage, class Allocator>
void 
Matrix<T, RowStorage, Allocator>::
RangeCheck(size_t i, size_t j) const
{
  // With optimization on we need speed, speed, speed (sorry, this is numerics),
  // so this check takes place only during development.
#ifndef NDEBUG
  if ((i > nrows_) || (i == 0))
    {
      throw std::range_error
        (std::string
         ("Matrix<T, RowStorage, Allocator>::RangeCheck: index i is out of range: ")
         + boost::lexical_cast<std::string>(i));
    }
  
  if ((j > ncols_) || (j == 0))
    {
      throw std::range_error
        (std::string
         ("Matrix<T, RowStorage, Allocator>::RangeCheck: index j is out of range: ")
         + boost::lexical_cast<std::string>(j));
    }
#endif
}


template<class T, class RowStorage, class Allocator>
const T&
Matrix<T, RowStorage, Allocator>::
Zero() const
{
  // We cannot rely on default operator of T, since T maybe does not initialize
  // the data. FIXIT: compile-time check the existence of constructors of T
  // or whether T is a builtin-type
  static const T Zero = T(0); 
  return Zero;
}


template<class T, class RowStorage, class Allocator>
void 
Matrix<T, RowStorage, Allocator>::
UpdateColumnInfo(size_t row_index, 
                 const RowStorage& OldRow,
                 const RowStorage& NewRow)
{
  IndexStorage OldKeys, NewKeys;
  
  OldKeys.reserve(OldRow.size());
  NewKeys.reserve(NewRow.size());

  std::transform(OldRow.begin(), OldRow.end(),
                 std::back_inserter(OldKeys),
                 Private::select1st<typename RowStorage::value_type>());

  std::transform(NewRow.begin(), NewRow.end(),
                 std::back_inserter(NewKeys),
                 Private::select1st<typename RowStorage::value_type>());
  

  // maybe RowStorage misses the sorted property of std::map, so we 
  // are somewhat failsafe here. FIXIT: check profiler comment on this.
  std::sort(OldKeys.begin(), OldKeys.end());
  std::sort(NewKeys.begin(), NewKeys.end());


  IndexStorage ToErase, ToAdd;

  std::set_difference(OldKeys.begin(), OldKeys.end(),
                      NewKeys.begin(), NewKeys.end(),
                      std::back_inserter(ToErase));

  std::set_difference(NewKeys.begin(), NewKeys.end(),
                      OldKeys.begin(), OldKeys.end(),
                      std::back_inserter(ToAdd));

  
  typedef typename IndexStorage::iterator iterator;


  // erase the removed entries
  iterator to_erase_end = ToErase.end();
  for (iterator iter = ToErase.begin(); iter != to_erase_end; ++iter)
    {
      ColumnInfo_[*iter-1].erase(
                                 std::remove(ColumnInfo_[*iter-1].begin(), 
                                             ColumnInfo_[*iter-1].end(),
                                             row_index)
                                 ); 
    }

  // add new entries
  iterator to_add_end = ToAdd.end();
  for (iterator iter = ToAdd.begin(); iter != to_add_end; ++iter)
    {
      ColumnInfo_[*iter-1].push_back(row_index);

      assert(std::count(ColumnInfo_[*iter-1].begin(), 
                        ColumnInfo_[*iter-1].end(),
                        row_index) == 1);
    }
}


template<class T, class RowStorage, class Allocator>
std::ostream& operator<< (std::ostream& os, 
                          const Matrix<T, RowStorage, Allocator>& M)
{
  os 
    // << "dimensions: [" << M.nrows() << "x" << M.ncols() << "]:\n" 
    << std::endl;
  

  // If entry is not used, we output a '-'
  size_t end = M.nrows() + 1;
  for (size_t i=1; i!= end; ++i) 
    {
      const RowStorage& Row = M(i);
      
      size_t end = M.ncols() + 1;
      for (size_t j=1; j!=end; ++j) 
        {
          typename RowStorage::const_iterator lb = Row.lower_bound(j);
          if(lb != Row.end() && !(Row.key_comp()(j, lb->first))) 
            {
              os << std::setw(12) << lb->second << "  ";
            } 
          else 
            {
              os << std::setw(12) << "-" << "  ";
            }
        }
      os << std::endl;
    }

  return os;
}


} // namespace Linalg

#endif // DAIXT_LINALG_MATRIX_INC
