#include "daixtrose/Daixt.h"

namespace MyOwn 
{
template <class T> class Matrix {};

struct MatrixExpression {};
}

namespace Daixt 
{ 
template <class T> 
struct Disambiguator<MyOwn::Matrix<T> > 
{
  static const bool is_specialized = true;
  typedef MyOwn::MatrixExpression Disambiguation;
};
}

int main()
{
  MyOwn::Matrix<double> M1;
  MyOwn::Matrix<int> M2;
  
  using namespace Daixt::DefaultOps;
  M1 + M2;
}
