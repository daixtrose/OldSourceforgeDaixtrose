//-*-c++-*-
#ifndef LINALG_L2_NORM
#define LINALG_L2_NORM

#include "daixtrose/Daixt.h"
#include "tiny/TinyMatAndVec.h"

#include <cmath>
#include <numeric>

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
      BlockVector Tmp = (*iter) * (*iter);
      BV += Tmp;
    }
  
  return 
    std::sqrt(std::accumulate(BV.begin(), BV.end(), 0.0));
  

//   Tmp = std::inner_product(V.begin(), V.end(), V.begin(), Tmp);
  
//   return 
//     std::sqrt(std::accumulate(Tmp.begin(), Tmp.end(), 0.0));
}



#endif
