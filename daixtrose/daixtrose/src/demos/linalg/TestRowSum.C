#include "linalg/Linalg.h"

#include <map>

int main()
{
  typedef TinyMat::TinyQuadraticMatrix<double, 3> BlockMatrix;
  typedef 
    Linalg::Matrix<BlockMatrix, std::map<std::size_t, BlockMatrix> > Matrix;


  typedef TinyVec::TinyVector<double, 3> BlockVector;
  typedef Linalg::Vector<BlockVector> Vector;

  using namespace Daixt::DefaultOps;

  BlockMatrix B = 1.0;
  B(1, 1) = 12345678902.0;
  B(2, 2) = 2.5;
  B(3, 3) = 3.0;

  Matrix M(4, 4);
  M(1, 1) = B;
  M(2, 3) = B;
  M(3, 2) = B;
  M(3, 3) = 5.0;
  M(3, 4) = B;
  M(4, 4) = B;

  Vector V1 = Linalg::RowSum(M);

  std::cerr << M << std::endl;
  std::cerr << V1 << std::endl;
  
}
