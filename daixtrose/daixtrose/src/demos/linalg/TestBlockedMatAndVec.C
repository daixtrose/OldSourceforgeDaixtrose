#include "linalg/Linalg.h"
#include "tiny/TinyMatAndVec.h"
#include "linalg/RowSum.h"

#include <map>

int main()
{
  typedef TinyMat::TinyQuadraticMatrix<double, 3> BlockMatrix;
  typedef Linalg::Matrix<BlockMatrix> Matrix;

  typedef TinyVec::TinyVector<double, 3> BlockVector;
  typedef Linalg::Vector<BlockVector> Vector;

  typedef Linalg::Matrix<BlockVector> VectorMatrix;


  using namespace Daixt::DefaultOps;

  BlockVector BV = 0.0;
  BV(1) = 1.0; 
  BV(2) = 2.0;
  BV(3) = 3.0;

  VectorMatrix M1(2, 2);
  M1(1, 1) = BV;
  M1(2, 2) = BV + BV;
  M1(1, 2) = 1.0;
  M1(2, 1) = M1(1, 2);

  std::cerr << "M1 = \n" << M1 << std::endl;

  Vector V1;
  V1 = RowSum(M1);

  std::cerr << "RowSum(M1) = \n" << V1 << std::endl;

  V1 += RowSum(Lump(M1));

  std::cerr << V1 << std::endl;


  BlockMatrix B = 0.0;
  B(1, 1) = 1.0;
  B(2, 2) = 2.0;
  B(3, 3) = 3.0;

  Matrix M2(4, 4);
  M2(1, 1) = B;
  M2(2, 1) = B;
  M2(2, 2) = B;
  M2(3, 3) = B;
//   M(3, 3) = 5.0;
//   M(3, 4) = B;
  M2(4, 4) = B;

  std::cerr << "M2 = \n" << M2 << std::endl;
  Vector V2;
  V2 = RowSum(M2);
  std::cerr << "RowSum(M2) = " << V2;

  V2 = M2 * V2;
  
  std::cerr << "\nM2 * V2 =\n" << V2;

  V2 = - (M2 * V2) + V2;
  std::cerr << "\n- (M2 * V2) + V2 = \n" << V2;

  V2 = (- M2) * V2;
  std::cerr << "\n(- M2) * V2 = " << V2;
}
