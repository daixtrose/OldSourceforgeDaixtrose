#include "daixtrose/Daixt.h"

namespace MyOwn 
{

struct MatrixExpression {};
struct VectorExpression {};

template <class T> class Matrix 
{
public:
  typedef MatrixExpression Disambiguation;
};

template <class T> class Vector 
{
public:
  typedef VectorExpression Disambiguation;
};

// define a new Daixtrose-compatible unary functions 
DAIXT_DEFINE_UNOP_AND_FUNCTION(RowSum, RowSumOfMatrix, DoNothing)

} // namespace MyOwn 


namespace Daixt 
{
// Matrix * Vector = Vector
template <>
struct BinOpResultDisambiguator<MyOwn::MatrixExpression,
                                MyOwn::VectorExpression,
                                Daixt::DefaultOps::BinaryMultiply>  
{
  typedef MyOwn::VectorExpression Disambiguation;
}; 

template <>
struct UnOpResultDisambiguator<MyOwn::MatrixExpression,
                               MyOwn::RowSumOfMatrix>  
{
  typedef MyOwn::VectorExpression Disambiguation;
}; 

} // namespace Daixt 

int main()
{
  MyOwn::Matrix<double> M;
  MyOwn::Vector<double> V1;
  MyOwn::Vector<int> V2;
  
  using namespace Daixt::DefaultOps;
  V1 + M * V2 - MyOwn::RowSum(M);
}
