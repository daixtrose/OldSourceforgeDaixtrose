//-*-C++-*- 
// copyright 2003 by Markus Werle

#ifndef DAIXT_LINALG_SLICE_ITERATOR_INC
#define DAIXT_LINALG_SLICE_ITERATOR_INC

#include <cstddef> 
#include <iterator>
#include <iostream>

namespace Linalg
{
  // Implementation of a sliced iterator which handles a normal iterator with
  // regard to a given stride function

template <class T> 
class SliceIterator
{
public:
  typedef typename std::iterator_traits<T>::iterator_category iterator_category;
  typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  inline SliceIterator(T iter, std::size_t stride);

  inline reference operator*() const;

  typedef SliceIterator<T> MyOwnT;

  inline MyOwnT& operator++();    // preincrement
  inline MyOwnT  operator++(int); // postincrement
  inline MyOwnT& operator--();
  inline MyOwnT  operator--(int);
  inline MyOwnT& operator+=(difference_type Offset);
  inline MyOwnT  operator+(difference_type Offset);
  inline MyOwnT& operator-=(difference_type Offset);
  inline MyOwnT  operator-(difference_type Offset) const;

  inline difference_type operator-(const MyOwnT Right) const;

  reference operator[](difference_type _Off) const;
  bool operator==(const MyOwnT& Other) const;
  bool operator!=(const MyOwnT& Other) const;
  bool operator<(const MyOwnT& Other) const;
  bool operator>(const MyOwnT& Other) const;
  bool operator<=(const MyOwnT& Other) const;
  bool operator>=(const MyOwnT& Other) const;

  template <class TT> 
  friend 
  SliceIterator<TT> 
  operator+(difference_type Offset, const SliceIterator<TT>& iter);

protected:
  T iter_;
  std::size_t stride_;
};


template <class T> 
SliceIterator<T>::
SliceIterator(T iter, std::size_t stride)
  :
  iter_(iter),
  stride_(stride)
{
}


template <class T> 
typename SliceIterator<T>::reference 
SliceIterator<T>::operator*() const
{
  return *iter_;
}


template <class T>
SliceIterator<T>&
SliceIterator<T>::operator++()
{
  iter_ += stride_;
  return *this;
}

template <class T>
SliceIterator<T>
SliceIterator<T>::operator++(int)
{
  SliceIterator<T> Tmp = *this;
  iter_ += stride_;
  return Tmp;
}


template <class T>
SliceIterator<T>&
SliceIterator<T>::operator--()
{
  iter_ -= stride_;
  return *this;
}


template <class T>
SliceIterator<T>
SliceIterator<T>::operator--(int)
{
  SliceIterator<T> Tmp = *this;
  iter_ -= stride_;
  return Tmp;
}


template <class T>
SliceIterator<T>&
SliceIterator<T>::operator+=(SliceIterator<T>::difference_type Offset)
{
  iter_ += stride_ * Offset;
  return *this;
}


template <class T>
SliceIterator<T>
SliceIterator<T>::operator+(SliceIterator<T>::difference_type Offset)
{
  SliceIterator<T> Tmp = *this;
  Tmp += Offset;
  return Tmp;
}


template <class T>
SliceIterator<T>&
SliceIterator<T>::operator-=(SliceIterator<T>::difference_type Offset)
{
  iter_ -= stride_ * Offset;
  return *this;
}


template <class T>
SliceIterator<T>
SliceIterator<T>::operator-(SliceIterator<T>::difference_type Offset) const
{
  SliceIterator<T> Tmp = *this;
  Tmp -= Offset;
  return Tmp;
}


template <class T>
typename SliceIterator<T>::difference_type 
SliceIterator<T>::
operator-(const SliceIterator<T> Other) const
{
  // makes no sense when stride_ != Other.stride_
  assert(stride_ == Other.stride_);

  difference_type distance = iter_ - Other.iter_;

  return distance /= stride_;
}

template <class T>
typename SliceIterator<T>::reference
SliceIterator<T>::operator[](SliceIterator<T>::difference_type Offset) const
{
  return *(*this + Offset);
}


template <class T>
bool
SliceIterator<T>::operator==(const SliceIterator<T>& Other) const
{
  return iter_ == Other.iter_;
}


template <class T>
bool
SliceIterator<T>::operator!=(const SliceIterator<T>& Other) const
{
  return (iter_ != Other.iter_);
}


template <class T>
bool
SliceIterator<T>::operator<(const SliceIterator<T>& Other) const
{
  return iter_ < Other.iter_;
}


template <class T>
bool
SliceIterator<T>::operator>(const SliceIterator<T>& Other) const
{
  return iter_ > Other.iter_;
}


template <class T>
bool
SliceIterator<T>::operator<=(const SliceIterator<T>& Other) const
{
  return iter_ <= Other.iter_;
}


template <class T>
bool
SliceIterator<T>::operator>=(const SliceIterator<T>& Other) const
{
  return iter_ >= Other.iter_;
}


template <class T>
SliceIterator<T> 
operator+(typename SliceIterator<T>::difference_type Offset, 
          const SliceIterator<T>& iter)
{
  return iter + Offset;
}

  
} // namespace Linalg


#endif // DAIXT_LINALG_SLICE_ITERATOR_INC
