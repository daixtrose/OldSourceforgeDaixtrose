#include "linalg/Linalg.h"
#include "tiny/TinyMatAndVec.h"
#include "linalg/L2_Norm.h"


int main()
{
  typedef TinyVec::TinyVector<double, 3> BlockVector;
  typedef Linalg::Vector<BlockVector> Vector;
 
  using namespace Daixt::DefaultOps;

  BlockVector BV = 0.0;
  BV(1) = 1.0; 
  BV(2) = 2.0;
  BV(3) = 3.0;

  Vector V1(2);
  V1(1) = BV;
  V1(2) = BV + BV;

  std::cerr << "V1: " << V1 << std::endl;
  std::cerr << "L2_Norm(V1) = " << L2_Norm(V1) << "\n";
  std::cerr << "L2_Norm(V1 + V1) = " << L2_Norm(V1 + V1) << "\n";
}
