//-*-c++-*-
#ifndef LINALG_L2_NORM
#define LINALG_L2_NORM

#include "daixtrose/Daixt.h"
#include "tiny/TinyMatAndVec.h"
#include "linalg/RowAndColumCounters.h"
#include "linalg/RowAndColumExtractors.h"

#include <cmath>
#include <numeric>


namespace Linalg
{

template<class Vector> 
inline 
double
L2_Norm(const Vector& V)
{ 
  typedef typename Vector::value_type BlockVector;
  typedef typename Vector::const_iterator const_iterator;
  
  BlockVector BV = 0;
  using namespace Daixt::DefaultOps;

  const_iterator end = V.end();
  for (const_iterator iter = V.begin(); iter != end; ++iter)
    {
      BV += (*iter) * (*iter);
    }
  
  return 
    std::sqrt(std::accumulate(BV.begin(), BV.end(), 0.0));
}


// FIXIT: works only for block vectors
template<class T> 
inline 
double
L2_Norm(const Daixt::Expr<T>& E)
{ 
  typedef typename Daixt::Expr<T>::Disambiguation Disambiguation;
  typedef typename Disambiguation::NumT BlockVector;
  
  std::size_t nrows = NumberOfRows(E);
  
  BlockVector BV = 0;
  using namespace Daixt::DefaultOps;


  for (std::size_t j = 1; j != nrows + 1; ++j)
    {
      BlockVector const & Entry = RowExtractor<Disambiguation>(j)(E);
      BV += Entry * Entry;
    }
  
  return 
    std::sqrt(std::accumulate(BV.begin(), BV.end(), 0.0));
}


} // namespace Linalg

#endif
