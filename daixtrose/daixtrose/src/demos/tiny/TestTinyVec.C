#include <iostream>
#include "tiny/TinyMatAndVec.h"

int main()
{
  using namespace Daixt::OutputUtilities;
  using namespace Daixt::DefaultOps;

  using namespace TinyMatAndVec;

  typedef TinyVector<double, 3> TinyVec;
  typedef TinyQuadraticMatrix<double, 3> TinyMat;

  TinyVec V1 = 5.0;
  TinyVec V2 = 3.0;

  std::cerr << V1 << std::endl;
  std::cerr << V2 << std::endl;

  TinyVec V3 = V1 + V2;
  std::cerr << V3 << std::endl;

  V3(2) = 17;
  std::cerr << V3 << std::endl;

//   V3 = V3 / V2;
//   std::cerr << V3 << std::endl;


  TinyMat M = 0.0;
  std::cerr << M << std::endl;

  M(1, 1) = 1.0;
  M(2, 2) = 2.0;
  M(3, 3) = 2.0;

  V2 = 1.0;
  V3 = M * M * V2;
  
  std::cerr << M << '\n' << V3 << std::endl;
}
