#include <iostream>
#include "tiny/TinyMatAndVec.h"

int main()
{
  using namespace Daixt::OutputUtilities;
  using namespace Daixt::DefaultOps;

  typedef TinyMat::TinyQuadraticMatrix<double, 3> TinyMat;

  TinyMat M1 = 5.0;
  TinyMat M2 = 3.0;

  TinyMat M3 = M1 + M2;
  std::cerr << M3 << std::endl;

  M3 = Lump(M1 * M2);
  M3(1, 2) = 17;
  std::cerr << M3 << std::endl;

  M3 = Transpose(M3);
  std::cerr << M3 << std::endl;

  TinyMat M4 = 0;
  std::cerr << M4 << "\n" << M1 << std::endl;
  M4 += M1;
  std::cerr << M4 << std::endl;

  M4 -= 5.0;
  std::cerr << M4 << std::endl;

  M4 += 42.0;
  std::cerr << M4 << std::endl;

  M4 *= 3.0 / 42.0;
  std::cerr << M4 << std::endl;

}
