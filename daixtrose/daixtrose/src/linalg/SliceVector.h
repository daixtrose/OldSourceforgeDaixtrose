//-*-C++-*- 
#ifndef DAIXT_LINALG_SLICE_VECTOR_INC
#define DAIXT_LINALG_SLICE_VECTOR_INC

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/if.hpp"
#include "boost/lambda/exceptions.hpp"
#include "boost/lambda/casts.hpp"
#include "boost/lambda/algorithm.hpp"
#include "boost/lambda/bind.hpp"

#include "linalg/Vector.h"
#include "linalg/SliceIterator.h"

#include <cstddef>
#include <cassert>
#include <algorithm>
#include <valarray>  // we reuse std::slice
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
// A sliced vector implementation, similar to valarray's slice_array, but a
// little bit more flexible (and less exception safe, maybe ...)
////////////////////////////////////////////////////////////////////////////////
//
// WARNING: a lot of differences to std::slice_array<T>!
//
// We do not expose private parts of the referenced Vector, but forward all
// requests to the public interface of template argument "Vector".


namespace Linalg
{

template <
          class Vector, 
          std::size_t BaseIndex = 0,
          class Slice           = std::slice
          >
class SliceVector
{
public:
  // disambiguation
  typedef typename Vector::value_type value_type;
  typedef typename Vector::allocator_type Allocator;
  typedef typename Vector::size_type size_type;

  // convenience
  typedef SliceVector<Vector, BaseIndex, Slice> MyOwnType;

  // plug into Daixt
  typedef VectorExpression<VectorDisambiguator<value_type, Allocator> > 
  Disambiguation;

  //////////////////////////////////////////////////////////////////////////////
  // provide STL with a minimum of information

  typedef SliceIterator<typename Vector::iterator> iterator;
  typedef SliceIterator<typename Vector::const_iterator> const_iterator;

  inline iterator begin();
  inline const_iterator begin() const;
  inline iterator end();
  inline const_iterator end() const;

  inline size_t size() const;

  //////////////////////////////////////////////////////////////////////////////

  inline SliceVector(Vector& V, const Slice& s);

  inline SliceVector(const MyOwnType& Other);

  inline MyOwnType& operator=(const MyOwnType& Other);
  inline MyOwnType& operator=(const value_type& t);


  // the next 3 memfuns work only with types that have the same base index

  template <class OtherT> 
  inline MyOwnType& operator=(const OtherT& Other);

  template <class OtherT> 
  inline MyOwnType& operator+=(const OtherT& Other);

  template <class OtherT>
  inline MyOwnType& operator-=(const OtherT& Other);


  inline void swap(MyOwnType& Other);

  inline ~SliceVector();

  inline value_type& operator()(size_t j);
  inline const value_type& operator()(size_t j) const;

private:
  inline void RangeCheck(size_t j) const; 

  Vector* data_;
  Slice* slice_; // using a pointer here saves us from a lot of exception
  // safety issues. OTOH the myriad of RowExtractor constructor calls below
  // which might throw in case of insufficient memory make this class, well, not
  // as good as it could be. Sorry, Herb, I hope for optimiters flattening that
  // stuff anyway, so I restart to care when I get bitten by this ;-)

  // And yes, that's political incorrect ...
};


////////////////////////////////////////////////////////////////////////////////
// data output
template <class Vector, std::size_t BaseIndex, class Slice>
inline
std::ostream& operator<< (std::ostream& o, 
                          const SliceVector<Vector, BaseIndex, Slice>& V);


////////////////////////////////////////////////////////////////////////////////
// this one is a convenience wrapper used by the solver
// FIXIT: does not belong here, will go elsewhere later
template <class Vector, std::size_t BaseIndex, class Slice> 
double GetValue(const SliceVector<Vector, BaseIndex, Slice>& V, size_t j)
{
  return V(j);
}


////////////////////////////////////////////////////////////////////////////////
// **************************** IMPLEMENTATION *******************************//
////////////////////////////////////////////////////////////////////////////////

template <class Vector, std::size_t BaseIndex, class Slice> 
typename SliceVector<Vector, BaseIndex, Slice>::iterator 
SliceVector<Vector, BaseIndex, Slice>::
begin()
{
  // We change the semantics of slice_->start() (well at least for vectors,
  // where the first valid index is not 0 as in std::vector<T>). We say:
  // slice_->start() is interpreted as an offset, so if we want the first
  // element of data_ returned by this function slice_->start() must be 0
  //
  // Like this we assure that You do not have to change the slice indices if You
  // switch the vector to be sliced. 

  return iterator(data_->begin() + slice_->start(), slice_->stride());
}

template <class Vector, std::size_t BaseIndex, class Slice>
typename SliceVector<Vector, BaseIndex, Slice>::const_iterator 
SliceVector<Vector, BaseIndex, Slice>::
begin() const
{
  return const_iterator(data_->begin() + slice_->start(), slice_->stride());
}

template <class Vector, std::size_t BaseIndex, class Slice>
typename SliceVector<Vector, BaseIndex, Slice>::iterator
SliceVector<Vector, BaseIndex, Slice>::
end()
{
  return iterator(
                  data_->begin() 
                  + 
                  slice_->start() 
                  + 
                  (slice_->size() * slice_->stride()),

                  slice_->stride()
                  );  
}

template <class Vector, std::size_t BaseIndex, class Slice>
typename SliceVector<Vector, BaseIndex, Slice>::const_iterator
SliceVector<Vector, BaseIndex, Slice>::
end() const
{
  return const_iterator(
                        data_->begin() 
                        + 
                        slice_->start() 
                        + 
                        (slice_->size() * slice_->stride()),
                        
                        slice_->stride()
                        );  
}


template <class Vector, std::size_t BaseIndex, class Slice>
size_t
SliceVector<Vector, BaseIndex, Slice>::
size() const 
{
  return slice_->size();
}

template <class Vector, std::size_t BaseIndex, class Slice>
SliceVector<Vector, BaseIndex, Slice>::
SliceVector(Vector& V, const Slice& s) 
  : 
  data_(&V),
  slice_(new Slice(s))
{
#ifndef NDEBUG
  if (slice_->size() == 0)
    {
      throw std::range_error
        ("SliceVector<Vector, BaseIndex, Slice>::SliceVector(Vector& V,"
         " Slice s):\nslice_->size() == 0 makes no sense to me");
    }

  if (slice_->stride() == 0)
    {
      throw std::range_error
        ("SliceVector<Vector, BaseIndex, Slice>::SliceVector(Vector& V,"
         " Slice s):\nslice_->stride() == 0 makes no sense to me");
    }
  
  if (V.end()  
      < 
      (
       data_->begin() 
       + 
       slice_->start() 
       + 
       ((slice_->size() - 1) * slice_->stride())
       ))
    {
      throw std::range_error
        ("SliceVector<Vector, BaseIndex, Slice>::SliceVector(Vector& V,"
         " Slice s):\nrange of slice exceeds extent of V");
    }
#endif // NDEBUG
}



template <class Vector, std::size_t BaseIndex, class Slice>
SliceVector<Vector, BaseIndex, Slice>::
SliceVector(const SliceVector<Vector, BaseIndex, Slice>& Other)
  :
  data_(Other.data_),
  slice_(Other.slice_)
{
}


template <class Vector, std::size_t BaseIndex, class Slice>
SliceVector<Vector, BaseIndex, Slice>& 
SliceVector<Vector, BaseIndex, Slice>::
operator=(const SliceVector<Vector, BaseIndex, Slice>& Other)
{
  if (this != &Other)
    {
      data_ = Other.data_; 
      slice_ = Other.slice_; 
    }
  return *this;
}


template <class Vector, std::size_t BaseIndex, class Slice>
SliceVector<Vector, BaseIndex, Slice>& 
SliceVector<Vector, BaseIndex, Slice>::
operator=(const value_type& t)
{
  std::fill(this->begin(), this->end(), t); 
  return *this;
}


template <class Vector, std::size_t BaseIndex, class Slice>
template <class OtherT>
SliceVector<Vector, BaseIndex, Slice>& 
SliceVector<Vector, BaseIndex, Slice>::
operator=(const OtherT& Other)
{
  size_t nrows = NumberOfRows(Other);
  
#ifndef NDEBUG
  if (nrows != this->size())
    throw 
      std::range_error("SliceVector<Vector, BaseIndex, Slice>::operator=: "
                       "the number of rows is incompatible with this->size()");
#endif

  // The check for whether we need a temporary could be refined: if *this does
  // not occur on the rhs of a multiplication, the temporary might not be
  // required.

  // assuming Other has base BaseIndex
  if (Daixt::CountOccurrence(Other, *this)) // must use a temporary
    {
      // copy to temporary
      Vector Tmp(nrows); 
      
      typename Vector::iterator iter = Tmp.begin();
      for (size_t i = 0; i != nrows; ++i, ++iter)
        {
          *iter = RowExtractor<Disambiguation>(i + BaseIndex)(Other);
        }
      
      // assign temporary to myself
      std::copy(Tmp.begin(), Tmp.end(), this->begin());
    }
  else 
    {
      iterator iter = this->begin();
      for (size_t i = 0; i != nrows; ++i, ++iter)
        {
          *iter = RowExtractor<Disambiguation>(i + BaseIndex)(Other);
        }
    }

  return *this;
}


template <class Vector, std::size_t BaseIndex, class Slice>
template <class OtherT>
SliceVector<Vector, BaseIndex, Slice>& 
SliceVector<Vector, BaseIndex, Slice>::
operator+=(const OtherT& Other)
{
  size_t nrows = NumberOfRows(Other);

#ifndef NDEBUG
  if (nrows != this->size()) 
    throw 
      std::range_error("SliceVector<Vector, BaseIndex, Slice>::operator+=: "
                       "the number of rows is incompatible");
#endif

  if (Daixt::CountOccurrence(Other, *this)) 
    {
      // use temporary
      Vector Tmp(nrows); 
      
      typename Vector::iterator iter = Tmp.begin();
      for (size_t i = 0; i != nrows; ++i, ++iter)
        {
          *iter = RowExtractor<Disambiguation>(i + BaseIndex)(Other);
        }
      
      // add temporary to myself
      std::transform(this->begin(), this->end(), 
                     Tmp.begin(), 
                     this->begin(),
                     std::plus<typename Vector::value_type>());
    }
  else
    {
      iterator iter = this->begin();
      for (size_t i = 0; i != nrows; ++i, ++iter)
        {
          *iter += RowExtractor<Disambiguation>(i + BaseIndex)(Other);
        }
    }

  return *this;
}


template <class Vector, std::size_t BaseIndex, class Slice>
template <class OtherT>
SliceVector<Vector, BaseIndex, Slice>& 
SliceVector<Vector, BaseIndex, Slice>::
operator-=(const OtherT& Other)
{
  // delegate to +=
  //using Daixt::DefaultOps::operator-;
  using namespace Daixt::DefaultOps;
  return this->operator+=(Daixt::ExprManip::Simplify(- Other));
}


template <class Vector, std::size_t BaseIndex, class Slice>
void
SliceVector<Vector, BaseIndex, Slice>::
swap(SliceVector<Vector, BaseIndex, Slice>& Other) 
{
  std::swap(slice_, Other.slice_);
  std::swap(data_, Other.data_);
}


template <class Vector, std::size_t BaseIndex, class Slice>
SliceVector<Vector, BaseIndex, Slice>::~SliceVector() 
{
  // do NOT delete data_, since we are not owner, but
  delete slice_;
}


template <class Vector, std::size_t BaseIndex, class Slice>
typename SliceVector<Vector, BaseIndex, Slice>::value_type&
SliceVector<Vector, BaseIndex, Slice>::
operator()(size_t j) 
{
  RangeCheck(j);
  return *(this->begin() + j - BaseIndex);
} 
  

template <class Vector, std::size_t BaseIndex, class Slice>
const typename SliceVector<Vector, BaseIndex, Slice>::value_type&
SliceVector<Vector, BaseIndex, Slice>::
operator()(size_t j) const 
{
  RangeCheck(j);
  return *(this->begin() + j - BaseIndex);
}


template <class Vector, std::size_t BaseIndex, class Slice>
void 
SliceVector<Vector, BaseIndex, Slice>::
RangeCheck(size_t j) const
{
#ifndef NDEBUG
  if ((j + 1 < BaseIndex + 1) // the +1 calms down compilers when BaseIndex == 0
      || 
      (BaseIndex + this->size() <= j))
    throw 
      std::range_error
      ("SliceVector<Vector, BaseIndex, Slice>::RangeCheck: index j is out of range");

#endif
}


template <class Vector, std::size_t BaseIndex, class Slice>
std::ostream& operator<< (std::ostream& os, 
                          const SliceVector<Vector, BaseIndex, Slice>& V)
{
  using boost::lambda::_1;
  using boost::lambda::constant;

  std::for_each(V.begin(), V.end()
                (os.width(10),
                 os << _1 << constant('\n')));


//   typedef typename SliceVector<Vector, BaseIndex, Slice>::iterator 
//     iter = this->begin();

  

//   for (std::size_t i = 0; i != nrows; ++i, ++iter)
//     {
//       os.width(10);
//       os << *iter << std::endl;
//     }

  return os;
}



} // namespace Linalg




#endif // DAIXT_LINALG_SLICE_VECTOR_INC
