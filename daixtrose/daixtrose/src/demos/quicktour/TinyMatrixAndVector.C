#include <iostream>
#include "tiny/TinyMatAndVec.h"

int main()
{
  // all features are hidden in sub-namespaces and have to be made
  // visible when needed, here we want to use the output utilities and
  // we want to work with matrix-vector expressions
  using namespace Daixt::OutputUtilities;
  using namespace Daixt::DefaultOps;

  // a shorthand saves us from a lot of typing
  using namespace TinyMatAndVec;
  typedef TinyVector<double, 3> TinyVec;
  typedef TinyQuadraticMatrix<double, 3> TinyMat;

  TinyMat M1; // no data initialization (for efficiency reasons)
  TinyMat M2 = 3.0; // all entries are initialized with the value

  M1 = 2.0; // set all entries at once

  M1(2, 1) = 15.99; // The index operator 

  TinyMat M3 = M2 + Transpose(M1);

  // Some of the things one expects from a tiny matrix lib
  M3 *= 0.5;
  M3 -= 1.0;

  // note that the next operation does not produce any temporaries and
  // therefore the rhs might be completely optimized away 
  TinyMat M4 = M1 * M3;

  // pretty print output 
  std::cerr << M1 << M2 << M3 << M4 << '\n'; 

  TinyVec V1 = 5.0;
  V1(2) = 0.0;

  // no temporaries - and a looong compile time ;-)
  TinyVec V2 = M1 * M2 * (M3 - M4) * V1; 
  
  // delimiters required
  std::cerr << (V2 == M1 * M2 * (M3 - M4) * V1); 
}
  
