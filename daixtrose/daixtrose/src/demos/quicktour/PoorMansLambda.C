#include "daixtrose/Daixt.h"

#ifdef USE_BOOST_LAMBDA
// ------------------------------------------------------------
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/if.hpp"
#include "boost/lambda/exceptions.hpp"
#include "boost/lambda/casts.hpp"
#include "boost/lambda/algorithm.hpp"
#include "boost/lambda/bind.hpp"

#else
// ------------------------------------------------------------

#include "boost/mpl/count.hpp"
#include "boost/mpl/list.hpp"


//////////////////////////////////////////////////////////////////////////////
// Poor man's lambda library 

namespace MyLambda
{
struct Comparison {};

struct X {
  typedef Comparison Disambiguation;
};

template <class T>
class Evaluator
{
private:
  T Value_;

  // handle placeholder
  inline T 
  operator()(const X& x) const 
  { 
    return Value_; 
  }

  // reach through
  template <class C> 
  inline const C&
  operator()(const C& c) const
  {
    return c;
  }

  // decide on return type 
  template <class OP> struct ReturnType
  {
    typedef typename boost::mpl::list<
      Daixt::DefaultOps::BinaryPlus,
      Daixt::DefaultOps::BinaryMinus,
      Daixt::DefaultOps::BinaryMultiply,
      Daixt::DefaultOps::BinaryDivide,
      Daixt::DefaultOps::BinaryModuloAliasRemainder,
      Daixt::DefaultOps::UnaryPlus,
      Daixt::DefaultOps::UnaryMinus,
      Daixt::DefaultOps::UnaryPreIncrement,
      Daixt::DefaultOps::UnaryPreDecrement,
      Daixt::DefaultOps::UnaryComplement
      // to be continued (sin, cos, etc ...)
      > ArithmeticBinOps;
    
    typedef typename boost::mpl::count<ArithmeticBinOps, OP>::type n;
    typedef typename boost::mpl::if_<n, T, bool>::type type;
  };

public:
  inline Evaluator(const T& Value) : Value_(Value) {} 

  template <class ARG, class OP>
  inline typename ReturnType<OP>::type
  operator()(const Daixt::UnOp<ARG, OP>& UO) const  
  {
    return OP::Apply((*this)(UO.arg()), 
                     Daixt::Hint<typename ReturnType<OP>::type>());
  }
  
  template <class LHS, class RHS, class OP>
  inline typename ReturnType<OP>::type
  operator()(const Daixt::BinOp<LHS, RHS, OP>& BO) const  
  {
    return OP::Apply((*this)(BO.lhs()), 
                     (*this)(BO.rhs()), 
                     Daixt::Hint<typename ReturnType<OP>::type>());
  }

};

} // namespace MyLambda


// communicate with Daixtrose
namespace Daixt
{
// disambiguation (the odd stuff for small examples, but useful
// if You do a lot of different Expression Template stuff in the
// same translation unit
template <class T, class OP>
struct BinOpResultDisambiguator<MyLambda::Comparison, T, OP>
{
  typedef MyLambda::Comparison Disambiguation;
};

template <class T, class OP>
struct BinOpResultDisambiguator<T, MyLambda::Comparison, OP>
{
  typedef MyLambda::Comparison Disambiguation;
};

template <class OP>
struct BinOpResultDisambiguator<MyLambda::Comparison, MyLambda::Comparison, 
OP>
{
  typedef MyLambda::Comparison Disambiguation;
};

// operator() for use with STL
template <class T> 
class FeaturesOfExpression<MyLambda::Comparison, T>
{
public:
  template <class D>
  bool operator()(const D& d) 
  {
    return MyLambda::Evaluator<D>(d)
      (Daixt::unwrap_expr(static_cast<const T&>(*this)));
  }

  virtual ~FeaturesOfExpression() {};
};

} // namespace Daixt

#endif // USE_BOOST_LAMBDA



////////////////////////////////////////////////////////////////////////////
template<class InputIterator, class OutputIterator, class Predicate>
OutputIterator copy_if(InputIterator begin, InputIterator end,
                       OutputIterator destBegin, Predicate p)
{
        while (begin != end)
        {
                if (p(*begin))
                        *destBegin++ = *begin;
                ++begin;
        }
        return destBegin;
}


#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>


int main()
{
#ifdef USE_BOOST_LAMBDA
  boost::lambda::placeholder1_type x;
#else
  MyLambda::X x;
#endif

  std::vector<int> v;
  for (int i = 0; i <= 20; ++i)
    v.push_back(i);
  
  std::vector<int>::iterator b(v.begin());
  std::vector<int>::iterator e(v.end());
  std::ostream_iterator<int> out(std::cout, " ");
  
  using namespace Daixt::DefaultOps;
  using namespace Daixt::DefaultOps::LogicalOps;

  std::cout << "All numbers     : "; std::copy(b, e, out);
  std::cout << '\n';
  
  std::cout << "x < 6           : "; copy_if(b, e, out, x < 6);
  std::cout << '\n';
  
  std::cout << "x > 15          : "; copy_if(b, e, out, x > 15);
  std::cout << '\n';
  
  std::cout << "x > 5 && x < 13 : "; copy_if(b, e, out, (x > 5) && (x < 
13));
  std::cout << '\n';
  
  std::cout << "2 * x == x + 5  : "; copy_if(b, e, out, 2 * x == x + 5);
  std::cout << '\n';
  
  std::cout << "3 * x != x * x  : "; copy_if(b, e, out, 3 * x != x * x);
  std::cout << '\n';
  
  std::cout << "!(x < 18)       : "; copy_if(b, e, out, !(x < 18));
  std::cout << '\n';
}
