//-*-C++-*- 
#ifndef DAIXT_LINALG_VECTOR_INC
#define DAIXT_LINALG_VECTOR_INC

#include "linalg/RowAndColumCounters.h"
#include "linalg/RowAndColumExtractors.h"
#include "linalg/Disambiguation.h"

#include "daixtrose/Daixt.h"

#include <cstddef>
#include <cassert>
#include <vector>
#include <algorithm>


////////////////////////////////////////////////////////////////////////////////
// An STL-based vector implementation
////////////////////////////////////////////////////////////////////////////////
//
// this is only an incomplete frontend to std::vector with base 1
// a lot of operations are missing

namespace Linalg
{

template <class T, class AllocatorT>
struct VectorDisambiguator
{
  typedef T NumT;
  typedef AllocatorT Allocator;
}; 


template <
          class T, 
          class Allocator = std::allocator<T> 
          >
class Vector 
{
public:
  // disambiguation
  typedef VectorExpression<VectorDisambiguator<T, Allocator> > Disambiguation;

  typedef Vector<T, Allocator> MyOwnType;
  typedef std::vector<T, Allocator> DataStorage;

  //////////////////////////////////////////////////////////////////////////////
  // provide STL with a minimum of information

  typedef std::size_t size_type;
  //typedef typename DataStorage::size_type size_type;

  typedef typename DataStorage::difference_type difference_type;

  typedef T value_type;
  // typedef typename DataStorage::value_type value_type;

  typedef typename DataStorage::const_reference const_reference;
  typedef typename DataStorage::reference reference;
  typedef typename DataStorage::const_pointer const_pointer;
  typedef typename DataStorage::pointer pointer;

  typedef Allocator allocator_type;

  typedef typename DataStorage::const_iterator const_iterator;
  typedef typename DataStorage::iterator iterator;


  //////////////////////////////////////////////////////////////////////////////

  inline iterator begin();
  inline const_iterator begin() const;
  inline iterator end();
  inline const_iterator end() const;

  inline size_type size() const;
  inline void resize(size_type NewSize);

  //////////////////////////////////////////////////////////////////////////////

  inline Vector(size_type n = 0);

  // We must provide this, otherwise template<class OtherT> inline Vector(const
  // OtherT& Other); will be called in "Vector V(2);"
  inline Vector(int n); 

  inline Vector(const MyOwnType& Other);
  template<class OtherT> inline Vector(const OtherT& Other);

  inline MyOwnType& operator=(const MyOwnType& Other);
  inline MyOwnType& operator=(const T& t);

  template<class OtherT> 
  inline MyOwnType& operator=(const OtherT& Other);

  template<class OtherT> 
  inline MyOwnType& operator+=(const OtherT& Other);

  template<class OtherT>
  inline MyOwnType& operator-=(const OtherT& Other);


  inline void swap(MyOwnType& Other);

  inline ~Vector();


  inline T& operator()(size_type j);
  inline const T& operator()(size_type j) const;

private:
  inline void RangeCheck(size_type j) const; 

  DataStorage data_;
};


////////////////////////////////////////////////////////////////////////////////
// data output
template<class T, class Allocator> 
inline
std::ostream& operator<< (std::ostream& o, 
                          const Vector<T, Allocator>& V);


////////////////////////////////////////////////////////////////////////////////
// this one is a convenience wrapper used by the solver
// FIXIT: does not belong here, will go elsewhere later
template <class T, class Allocator>
double GetValue(const Vector<T, Allocator>& V, typename Vector<T, Allocator>::size_type j)
{
  return V(j);
}


////////////////////////////////////////////////////////////////////////////////
// **************************** IMPLEMENTATION *******************************//
////////////////////////////////////////////////////////////////////////////////

template<class T, class Allocator> 
typename Vector<T, Allocator>::iterator 
Vector<T, Allocator>::
begin()
{
  return data_.begin();
}

template<class T, class Allocator>
typename Vector<T, Allocator>::const_iterator 
Vector<T, Allocator>::
begin() const
{
  return data_.begin();
}

template<class T, class Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::
end()
{
  return data_.end();  
}

template<class T, class Allocator>
typename Vector<T, Allocator>::const_iterator
Vector<T, Allocator>::
end() const
{
  return data_.end();
}


template<class T, class Allocator>
typename Vector<T, Allocator>::size_type
Vector<T, Allocator>::
size() const 
{
  return data_.size();
}


template<class T, class Allocator>
void 
Vector<T, Allocator>::
resize(typename Vector<T, Allocator>::size_type NewSize)
{
  data_.resize(NewSize);
}


template<class T, class Allocator>
Vector<T, Allocator>::
Vector(typename Vector<T, Allocator>::size_type size) 
  : 
  data_(size)
{}


template<class T, class Allocator>
Vector<T, Allocator>::
Vector(int size) 
  : 
  data_(size)
{}

template<class T, class Allocator>
Vector<T, Allocator>::
Vector(const Vector<T, Allocator>& Other)
  :
  data_()
{
  data_.reserve(Other.size());
  std::copy(Other.begin(), Other.end(), 
            std::back_inserter<DataStorage>(data_));
}


template<class T, class Allocator>
template<class OtherT>
Vector<T, Allocator>::
Vector(const OtherT& Other)
  :
  data_()
{
  size_type nrows = NumberOfRows(Other); 
  data_.reserve(nrows);

  for (size_type i = 0; i != nrows; ++i)
    {
      data_.push_back(RowExtractor<Disambiguation>(i+1)(Other));
    }
}


template<class T, class Allocator>
Vector<T, Allocator>& 
Vector<T, Allocator>::
operator=(const Vector<T, Allocator>& Other)
{
  if (this != &Other)
    {
      // trust in STL 
      data_ = Other.data_; 
    }
  return *this;
}


template<class T, class Allocator>
Vector<T, Allocator>& 
Vector<T, Allocator>::
operator=(const T& t)
{
  std::fill(data_.begin(), data_.end(), t); 
  return *this;
}


template<class T, class Allocator>
template<class OtherT>
Vector<T, Allocator>& 
Vector<T, Allocator>::
operator=(const OtherT& Other)
{
  size_type nrows = NumberOfRows(Other);

  // The check for whether we need a temporary could be refined: if *this does
  // not occur on the rhs of a multiplication, the temporary might not be
  // required.
  if (Daixt::CountOccurrence(Other, *this)) // must use a temporary
    {
      DataStorage Tmp; 
      Tmp.reserve(nrows);

      for (size_type i = 0; i != nrows; ++i)
        {
          Tmp.push_back(RowExtractor<Disambiguation>(i+1)(Other));
        }
      
      data_.swap(Tmp);
    }
  else // Not so many temporaries needed
    {
      data_.clear();
      data_.reserve(nrows); 

      for (size_type i = 0; i != nrows; ++i)
        {
          data_.push_back(RowExtractor<Disambiguation>(i+1)(Other));
        }
    }

  return *this;
}

template<class T, class Allocator>
template<class OtherT>
Vector<T, Allocator>& 
Vector<T, Allocator>::
operator+=(const OtherT& Other)
{
  size_type nrows = NumberOfRows(Other);

#ifndef NDEBUG
  if (nrows != data_.size()) 
    throw 
      std::range_error("Vector<T, Allocator>::operator+=: "
                       "the number of rows is incompatible");
#endif

  if (Daixt::CountOccurrence(Other, *this)) 
    {
      using namespace Daixt::DefaultOps;
      //using Daixt::DefaultOps::operator+; 
      *this = *this + Other; // delegate to operator= which uses temporary
    }
  else
    {
      for (size_type i = 0; i != nrows; ++i)
        {
          data_[i] += RowExtractor<Disambiguation>(i+1)(Other);
        }
    }

  return *this;
}


template<class T, class Allocator>
template<class OtherT>
Vector<T, Allocator>& 
Vector<T, Allocator>::
operator-=(const OtherT& Other)
{
  // delegate to +=
  //using Daixt::DefaultOps::operator-;
  using namespace Daixt::DefaultOps;
  return this->operator+=(Daixt::ExprManip::Simplify(- Other));
}


template<class T, class Allocator>
void
Vector<T, Allocator>::
swap(Vector<T, Allocator>& Other)
{
  data_.swap(Other.data_);
}


template<class T, class Allocator>
Vector<T, Allocator>::~Vector() 
{}


template<class T, class Allocator>
T&
Vector<T, Allocator>::
operator()(typename Vector<T, Allocator>::size_type j) 
{
  RangeCheck(j);
  return data_[j-1];
} 
  

template<class T, class Allocator>
const T&
Vector<T, Allocator>::
operator()(typename Vector<T, Allocator>::size_type j) const 
{
  RangeCheck(j);
  return data_[j-1];
}




template<class T, class Allocator>
void 
Vector<T, Allocator>::
RangeCheck(typename Vector<T, Allocator>::size_type j) const
{
#ifndef NDEBUG
  if ((j > data_.size()) || (j == 0)) 
    throw 
      std::range_error
      ("Vector<T, Allocator>::RangeCheck: index j is out of range");

#endif
}


template<class T, class Allocator>
std::ostream& operator<< (std::ostream& os, const Vector<T, Allocator>& V)
{
  typedef typename Vector<T, Allocator>::size_type size_type;

  for (size_type j=1; j!=V.size() + 1; ++j) 
    {
      os.width(10);
      os << V(j) << std::endl;
    }

  return os;
}



} // namespace Linalg




#endif // DAIXT_LINALG_VECTOR_INC
